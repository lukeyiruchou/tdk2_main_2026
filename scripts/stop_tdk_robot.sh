#!/bin/bash

# ================= 參數設定 =================
BASE_DIR="/home/$USER"
SLAM_DOCKER_DIR="$BASE_DIR/tdk_slam_ws/docker"
MAIN_DOCKER_DIR="$BASE_DIR/tdk2_main_2026/docker"
CAMERA_DOCKER_DIR="$BASE_DIR/TDK-straw"
SESSION="tdk_robot"
# ============================================

echo "=== 1. 終止 ROS 2 節點 ==="
if tmux has-session -t "$SESSION" 2>/dev/null; then
  echo "正在向所有 tmux 窗格發送中斷訊號 (Ctrl+C)..."
  for pane in $(tmux list-panes -s -t "$SESSION" -F "#{pane_id}"); do
    tmux send-keys -t "$pane" C-c
  done

  echo "等待節點釋放資源 (3秒)..."
  sleep 3

  echo "=== 2. 結束 tmux session ==="
  tmux kill-session -t "$SESSION" 2>/dev/null
  echo "監控視窗已關閉。"
else
  echo "[跳過] 未偵測到運行中的 tmux session: $SESSION"
fi

echo "=== 3. 停止 Docker 容器 ==="
if [ -d "$MAIN_DOCKER_DIR" ]; then
  echo "正在關閉 Main Docker ($MAIN_DOCKER_DIR)..."
  (cd "$MAIN_DOCKER_DIR" && docker compose down)
else
  echo "[警告] 找不到路徑: $MAIN_DOCKER_DIR"
fi

if [ -d "$SLAM_DOCKER_DIR" ]; then
  echo "正在關閉 SLAM Docker ($SLAM_DOCKER_DIR)..."
  (cd "$SLAM_DOCKER_DIR" && docker compose down)
else
  echo "[警告] 找不到路徑: $SLAM_DOCKER_DIR"
fi
if [ -d "$CAMERA_DOCKER_DIR" ]; then
  echo "正在關閉 CAMERA Docker ($CAMERA_DOCKER_DIR)..."
  (cd "$CAMERA_DOCKER_DIR" && docker compose down)
else
  echo "[警告] 找不到路徑: $CAMERA_DOCKER_DIR"
fi

echo ""
echo "=== 4. 系統關閉狀態確認 ==="
REMAINING_CONTAINERS=$(docker ps -q --filter "name=ros2_tdk" --filter "name=tdk_slam" --filter "name=tdk-straw")

if [ -z "$REMAINING_CONTAINERS" ]; then
  echo "[成功] 所有 ROS 容器與相關服務均已完全停止！"
else
  echo "[注意] 仍有容器在運行中，請手動檢查：$REMAINING_CONTAINERS"
fi

echo "=========================================="
read -p "請按 Enter 鍵關閉此視窗..."