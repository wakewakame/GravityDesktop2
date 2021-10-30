import CoreMotion
import SwiftUI

struct IMUPreview {
    let gravity         : (Double, Double, Double)
    let userAcceleration: (Double, Double, Double)
    let attitude        : (Double, Double, Double)
    let rotationRate    : (Double, Double, Double)
    let timestamp       : TimeInterval
    var estimated3Axis  : (Double, Double, Double) = (0.0, 0.0, 0.0)
    
    init() {
        gravity = (0.0, 0.0, 0.0)
        userAcceleration = (0.0, 0.0, 0.0)
        attitude = (0.0, 0.0, 0.0)
        rotationRate = (0.0, 0.0, 0.0)
        timestamp = 0.0
    }

    init(
        gravity: (Double, Double, Double),
        userAcceleration: (Double, Double, Double),
        attitude: (Double, Double, Double),
        rotationRate: (Double, Double, Double),
        timestamp: TimeInterval
    ) {
        self.gravity = gravity
        self.userAcceleration = userAcceleration
        self.attitude = attitude
        self.rotationRate = rotationRate
        self.timestamp = timestamp
    }
}

class Imu {
	// IMUにアクセスするためのクラス
	private let motionManager = CMMotionManager()

	private let encodeQueue: OperationQueue = {
		// 2つ以上のスレッドから同時にファイルへ書き込まれるとよくないので
		// OperationQueueの並列化はしない
		let queue = OperationQueue()
		queue.maxConcurrentOperationCount = 1
		return queue
	}()

	private var previewCallback: ((IMUPreview) -> ())? = nil

    init() {
		guard motionManager.isDeviceMotionAvailable else { return }
		motionManager.deviceMotionUpdateInterval = 0.001
		motionManager.startDeviceMotionUpdates(
			using: .xMagneticNorthZVertical,
			to: encodeQueue,
			withHandler: motionHandler
		)
    }

	deinit {
		motionManager.stopDeviceMotionUpdates()
	}

	// プレビューデータを受け取るコールバック関数の登録
	func preview(_ preview: ((IMUPreview) -> ())?) {
		previewCallback = preview
	}

	// IMUが更新されたときに呼ばれるメソッド
	func motionHandler(motion: CMDeviceMotion?, error: Error?) {
		guard let motion = motion, error == nil else { return }

		// プレビュー用のデータ作成
		let preview = IMUPreview(
			gravity: (motion.gravity.x, motion.gravity.y, motion.gravity.z),
			userAcceleration: (motion.userAcceleration.x, motion.userAcceleration.y, motion.userAcceleration.z),
			attitude: (motion.attitude.roll, motion.attitude.pitch, motion.attitude.yaw),
			rotationRate: (motion.rotationRate.x, motion.rotationRate.y, motion.rotationRate.z),
			timestamp: motion.timestamp
		)

        DispatchQueue.main.async { [weak self] in
            guard let self = self else { return }
            self.previewCallback?(preview)
        }
	}
}
