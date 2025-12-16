import mujoco
import mujoco.viewer
import numpy as np


class EelEnv:
    def __init__(self, xml_path="eel_simple.xml"):
        self.model = mujoco.MjModel.from_xml_path(xml_path)
        self.data = mujoco.MjData(self.model)

        self.viewer = None
        self.model.opt.gravity[:] = 0, 0, 0

        # ======== 流體參數（穩定版）========
        self.Cd = 0.0          # 再弱化阻力
        self.A  = 0.0      # 有效面積
        self.drag_scale = 0.0  # 額外弱化 20 倍

        # ======== 更輕的 mass scaling，避免身體僵硬 ========
        for bid in range(self.model.nbody):
            self.model.body_mass[bid] *= 1.5

        # ======== Body IDs ========
        self.segment_ids = [
            self.model.body('head').id,
            self.model.body('seg2').id,
            self.model.body('seg3').id,
            self.model.body('seg4').id,
            self.model.body('seg5').id,
            self.model.body('seg6').id,
        ]


    def reset(self):
        mujoco.mj_resetData(self.model, self.data)
        # 必須清零所有外力
        self.data.xfrc_applied[:] = 0
        return self.data


    # ===================================================
    # 🚫 完全關閉 Added Mass + 弱化 Drag（最穩定）
    # ===================================================
    def apply_fluid_forces(self):

        for bid in self.segment_ids:

            vel = self.data.cvel[bid, 3:6]
            speed = np.linalg.norm(vel)

            if speed < 1e-6:
                self.data.xfrc_applied[bid] = 0
                continue

            # ------ Quadratic Drag（弱化版）------
            drag = -self.drag_scale * self.Cd * self.A * speed * vel

            # ------ Added Mass 完全關閉 ------
            added = np.zeros(3)

            total = drag + added

            self.data.xfrc_applied[bid, 0:3] = total
            self.data.xfrc_applied[bid, 3:6] = 0


    def step(self, ctrl):
        self.data.ctrl[:] = ctrl
        mujoco.mj_step(self.model, self.data)
        self.apply_fluid_forces()


    def render(self):
        if self.viewer is None:
            self.viewer = mujoco.viewer.launch_passive(self.model, self.data)

            # self.viewer.cam.azimuth = 270
            # self.viewer.cam.elevation = -20
            # self.viewer.cam.distance = 1.2
            # self.viewer.cam.lookat[:] = [0.3, 0, 0]

        self.viewer.sync()


    def close(self):
        if self.viewer:
            self.viewer.close()
            self.viewer = None
