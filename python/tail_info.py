import trimesh

m = trimesh.load("tail_simple.stl")

print("bounds =")
print(m.bounds)

print("\ncenter =")
print(m.center_mass)

print("\nmin corner =", m.bounds[0])
