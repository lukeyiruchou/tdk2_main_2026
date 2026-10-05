#!/bin/bash

# ================= 參數設定 =================
BASE_DIR="/home/$USER"
MAP_NAME="carto_map_24"  # 請替換為預設地圖名稱（不含 .yaml）

SLAM_DOCKER_DIR="$BASE_DIR/tdk_slam_ws/docker"
MAIN_DOCKER_DIR="$BASE_DIR/tdk2_main_2026/docker"
CAMERA_DOCKER_DIR="$BASE_DIR/TDK-straw"
SESSION="tdk_robot"

# 觸控螢幕縮放倍率 (可依螢幕解析度調整: 1.2 ~ 1.8)
export GDK_DPI_SCALE=1.4
# ============================================

# ----------------------------------------------------
# 階段一：場地模式選擇
# ----------------------------------------------------
IS_MIRRORED=$(zenity --list --radiolist \
  --title="【步驟 1/2】TDK 場地模式設定" \
  --text="請選擇本場比賽場地模式：" \
  --column="選取" --column="值" --column="場地類型說明" \
  TRUE  "false" "🟢 一般場地 (Normal)  ->  path.yaml" \
  FALSE "true"  "🔴 鏡像場地 (Mirrored) ->  path_mirrored.yaml" \
  --hide-column=2 \
  --ok-label="下一步 ➡️" \
  --cancel-label="取消離開" \
  --width=600 --height=320)

if [ -z "$IS_MIRRORED" ]; then
  zenity --warning --title="操作取消" --text="未選擇場地模式，已取消啟動流程。" --width=350
  exit 0
fi

if [ "$IS_MIRRORED" = "true" ]; then
  MODE_TEXT="鏡像場地 (Mirrored)"
else
  MODE_TEXT="一般場地 (Normal)"
fi

# ----------------------------------------------------
# 階段二：任務排程選擇 (移除標籤，純文字 + 縮放)
# ----------------------------------------------------
SELECTED_MISSIONS=$(zenity --list --checklist \
  --title="【步驟 2/2】TDK 任務排程選擇" \
  --text="當前場地模式：${MODE_TEXT}\n請勾選本次要依序執行的任務：" \
  --column="勾選" --column="代號" --column="任務說明" \
  TRUE  "1"  "Mission 1 (任務一)" \
  FALSE "2"  "Mission 2 (任務二)" \
  TRUE  "3"  "Mission 3 (任務三)" \
  TRUE  "4F" "Mission 4 Front (任務四-前)" \
  FALSE "4B" "Mission 4 Back (任務四-後)" \
  --separator="," \
  --ok-label="啟動節點 🚀" \
  --cancel-label="上一步/取消" \
  --width=620 --height=450)

if [ -z "$SELECTED_MISSIONS" ]; then
  zenity --warning --title="操作取消" --text="未選擇任何任務，已取消啟動流程。" --width=350
  exit 0
fi

echo "=========================================="
echo " 場地模式: $MODE_TEXT"
echo " 任務排程: [ ${SELECTED_MISSIONS} ]"
echo "=========================================="

# ----------------------------------------------------
# 階段三：啟動 Docker 服務
# ----------------------------------------------------
echo "=== 1. 啟動 Docker 服務 ==="
cd "$SLAM_DOCKER_DIR" && docker compose up -d
cd "$MAIN_DOCKER_DIR" && docker compose up -d
cd "$CAMERA_DOCKER_DIR" && docker compose up -d

echo "=== 2. 等待容器啟動就緒 ==="
while ! docker ps --format '{{.Names}}' | grep -q "ros2_tdk" || \
      ! docker ps --format '{{.Names}}' | grep -q "tdk-straw" || \
      ! docker ps --format '{{.Names}}' | grep -q "tdk_slam"; do
  sleep 1
done
sleep 1

sudo chmod 666 /dev/ttyUSB* 2>/dev/null

# ----------------------------------------------------
# 階段四：定義終端執行指令
# ----------------------------------------------------
CMD_T1="cd ros2_ws && source install/setup.bash && sudo chmod 666 /dev/ttyUSB* 2>/dev/null; ros2 launch fsm_main robot_launch.py microros_port:=/dev/ttyUSB2 is_mirrored:=${IS_MIRRORED}"

CMD_T2="source /opt/ros/humble/setup.bash && source install/setup.bash && sudo chmod 666 /dev/ttyUSB* 2>/dev/null; ros2 launch tdk_slam_manager spawn_launch.py localization_mode:=cartographer"

CMD_T3="source /opt/ros/humble/setup.bash && source install/setup.bash && ros2 launch tdk_nav2_manager nav_launch.py map:=\$(ros2 pkg prefix tdk_slam_manager --share)/maps/${MAP_NAME}.yaml"

CMD_T4="source /opt/ros/humble/setup.bash && source install/setup.bash && ros2 launch straw_detector straw_with_camera.launch.py"


# ----------------------------------------------------
# 階段五：開啟 tmux 3 分割監控視窗
# ----------------------------------------------------
tmux kill-session -t "$SESSION" 2>/dev/null

tmux new-session -d -s "$SESSION" -n "ROS_MONITOR"
tmux set-option -g mouse on
tmux send-keys -t "$SESSION.0" "docker exec -it ros2_tdk bash -c '$CMD_T1; exec bash'" C-m

tmux split-window -v -t "$SESSION"
tmux send-keys -t "$SESSION.1" "docker exec -it tdk_slam bash -c '$CMD_T2; exec bash'" C-m

tmux split-window -h -t "$SESSION.1"
tmux send-keys -t "$SESSION.2" "docker exec -it tdk_slam bash -c '$CMD_T3; exec bash'" C-m

tmux split-window -h -t "$SESSION.2"
tmux send-keys -t "$SESSION.3" "docker exec -it tdk-straw bash -c '$CMD_T4; exec bash'" C-m

# 啟動終端機
gnome-terminal --title="TDK 機器人監控中心" -- tmux attach-session -t "$SESSION" &

# 讓終端機先獲取焦點
sleep 2


# ----------------------------------------------------
# 階段六：發車確認視窗
# ----------------------------------------------------
DIALOG_TITLE="TDK 發車確認"

# 視窗置頂守護進程
(
  for i in {1..20}; do
    if wmctrl -r "$DIALOG_TITLE" -b add,above 2>/dev/null; then
      break
    fi
    sleep 0.2
  done
) &

# 修正 Pango 標籤：使用標準 <span> foreground 屬性
CONFIRM_MSG="<big><b> 實機系統已就緒</b></big>\n\n場地模式：<b>${MODE_TEXT}</b>\n任務排程：<b>${SELECTED_MISSIONS}</b>\n\n<span foreground='#555555' size='small'>請確認終端機 Cartographer 與 Nav2 就緒後點擊發車。</span>"

# 呼叫 Zenity：拿掉易出錯的自訂 icon-name，使用系統原生執行圖示，並放寬視窗寬高
if zenity --question \
  --title="$DIALOG_TITLE" \
  --icon-name="dit_icon" \
  --text="$CONFIRM_MSG" \
  --ok-label="(灬ºωº灬) 確認發車 (GO)" \
  --cancel-label="❌ 僅監控 (不發車)" \
  --width=500 --height=260; then

  echo "=== 發送任務排程至 /fsm/trigger ==="
  docker exec ros2_tdk bash -c "
    cd ros2_ws/ &&
    source /opt/ros/humble/setup.bash &&
    source install/setup.bash &&
    ros2 topic pub --once -w 1 /fsm/trigger std_msgs/msg/String '{data: \"${SELECTED_MISSIONS}\"}'
  "
  echo "任務已成功發布！"
else
  echo "操作者取消發車，節點維持背景運行。"
fi