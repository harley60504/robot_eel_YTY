import trimesh
import numpy as np

# 讀取尾巴 STL
mesh = trimesh.load("tail_simple.stl")

# 取得 bounding box
bounds = mesh.bounds
min_pt = bounds[0]
max_pt = bounds[1]

print("Before fix bounds =", bounds)

# STL X 範圍本來是 0 → 0.08（原點在尾巴根部無誤）
# 但是目前尾巴偏移是因為模型「中心點」不在原點，需確保根部對齊 local origin

# 讓尾巴的 base（最左邊 X=0 的地方）對齊 STL 的原點
translation = -min_pt
translation[1] = 0  # y 不動
translation[2] = 0  # z 不動

print("Applying translation:", translation)

mesh.apply_translation(translation)

print("After fix bounds =", mesh.bounds)

mesh.export("tail_simple.stl")
print("已輸出：tail_simple_fixed.stl（尾巴根部 now at local origin）")
