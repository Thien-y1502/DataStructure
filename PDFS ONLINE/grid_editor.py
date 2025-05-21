

import numpy as np
import matplotlib.pyplot as plt

# Nhập kích thước lưới tọa độ từ người dùng
n = int(input("Nhập số hàng của lưới tọa độ: "))
m = int(input("Nhập số cột của lưới tọa độ: "))

# Nhập năng lượng ban đầu của robot
BR = int(input("Nhập năng lượng ban đầu của robot (BR): "))

# Initialize environment matrix: 0 for empty, 1 for obstacle, 2 for starting position
env = np.zeros((n, m), dtype=int)

# Create plot window
fig, ax = plt.subplots()
ax.set_xticks(np.arange(0, m + 1, 1))
ax.set_yticks(np.arange(0, n + 1, 1))
ax.set_xticklabels(np.arange(0, m + 1, 1))
ax.set_yticklabels(np.arange(0, n + 1, 1))
ax.tick_params(axis='both', which='both', length=0)  # Hide tick marks

# Display grid
plt.grid(True, which='both', color='black', linewidth=1)

# Track click counts for each cell
click_count = {}

def update_plot():
    ax.clear()
    ax.set_xticks(np.arange(0, m + 1, 1))
    ax.set_yticks(np.arange(0, n + 1, 1))
    ax.set_xticklabels(np.arange(0, m + 1, 1))
    ax.set_yticklabels(np.arange(0, n + 1, 1))
    ax.tick_params(axis='both', which='both', length=0)  # Hide tick marks

    # Draw environment matrix
    img = np.ones((n, m, 3))  # White for all squares
    img[env == 1] = [0, 0, 0]  # Black for obstacles
    img[env == 2] = [1, 0, 0]  # Red for starting position
    ax.imshow(img, extent=[0, m, 0, n], origin='lower')  # Change origin to 'lower'
    plt.grid(True, which='both', color='black', linewidth=1)
    plt.draw()

def on_click(event):
    global click_count
    if event.xdata is None or event.ydata is None:
        return  # Ignore clicks outside the grid

    # Calculate the grid cell based on click position
    x, y = int(event.ydata), int(event.xdata)
    if 0 <= x < n and 0 <= y < m:
        # Track clicks for the specific cell
        cell = (x, y)
        if cell not in click_count:
            click_count[cell] = 0
        click_count[cell] += 1

        # Double-click for obstacles
        if click_count[cell] == 2:
            env[x, y] = 1  # Mark as obstacle
            print(f"Obstacle set at ({x}, {y})")
            update_plot()

        # Triple-click for starting position
        elif click_count[cell] == 3:
            env[env == 2] = 0  # Clear previous starting position
            env[x, y] = 2  # Mark as starting position
            print(f"Starting position set at ({x}, {y})")
            update_plot()

        # Reset click count for the cell after processing
        if click_count[cell] >= 3:
            click_count[cell] = 0

def save_to_file():
    """
    Lưu ma trận vào file grid_matrix.txt, bao gồm kích thước n, m và năng lượng BR ở dòng đầu tiên.
    """
    flipped_env = np.flipud(env)  # Đảo ngược thứ tự các hàng để phù hợp với lưới tọa độ
    with open("grid_matrix.txt", "w") as file:
        # Ghi kích thước n, m và năng lượng BR vào dòng đầu tiên
        file.write(f"{n} {m} {BR}\n")
        # Ghi ma trận vào file
        for row in flipped_env:
            file.write(" ".join(map(str, row)) + "\n")
    print("Matrix, dimensions, and energy saved to grid_matrix.txt (flipped to match grid coordinates)")

# Initial plot
update_plot()

# Connect mouse events
fig.canvas.mpl_connect('button_press_event', on_click)

# Show plot
plt.show()

# Save matrix to file after closing the plot
save_to_file()