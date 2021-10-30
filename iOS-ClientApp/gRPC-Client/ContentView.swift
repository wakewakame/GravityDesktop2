import SwiftUI
import GRPC
import Logging
import NIO

class Client {
    private var conn: (
        group: MultiThreadedEventLoopGroup,
        channel: GRPCChannel,
        client: Imu_ImuClient?
    )? = nil
    deinit { disconnect() }
    func connect(_ host: String, _ port: Int) throws {
        disconnect()
        let group = MultiThreadedEventLoopGroup(numberOfThreads: 1)
        let channel = try GRPCChannelPool.with(
            target: .host(host, port: port),
            transportSecurity: .plaintext,
            eventLoopGroup: group
        )
        conn = (group: group, channel: channel, client: nil)
        conn!.client = Imu_ImuClient(channel: channel)
    }
    func disconnect() {
        try? conn?.channel.close().wait()
        try? conn?.group.syncShutdownGracefully()
        conn = nil
    }
    func send(_ prev: IMUPreview) -> Bool {
        guard let client = conn?.client else { return false }
        let request = Imu_ImuFrame.with {
            $0.timestamp = prev.timestamp
            $0.accX      = prev.gravity.0 + prev.userAcceleration.0
            $0.accY      = prev.gravity.1 + prev.userAcceleration.1
            $0.accZ      = prev.gravity.2 + prev.userAcceleration.2
            $0.roll      = prev.attitude.0
            $0.pitch     = prev.attitude.1
            $0.yaw       = prev.attitude.2
        }
        let _ = client.send(request)
        return true
    }
    func reset() -> Bool {
        guard let client = conn?.client else { return false }
        let _ = client.reset(Imu_Empty())
        return true
    }
}

struct MainView: View {
    private let imu = Imu()
    private var client = Client()
    @Binding private var host: String
    @Binding private var port: String
    @State private var prev: IMUPreview = IMUPreview()
    @State private var err: Bool = false
    @State private var err_desc: String = ""
    @Environment(\.presentationMode) var presentationMode
    init(_ host: Binding<String>, _ port: Binding<String>) {
        _host = host
        _port = port
    }
    var body: some View {
        ZStack {
            Color(UIColor.systemBackground)
            VStack(alignment: .leading, spacing: 6) {
                Text("timestamp: \(prev.timestamp)")
                Text("acc_x: \(prev.gravity.0 + prev.userAcceleration.0)")
                Text("acc_y: \(prev.gravity.1 + prev.userAcceleration.1)")
                Text("acc_z: \(prev.gravity.2 + prev.userAcceleration.2)")
                Text("roll: \(prev.attitude.0)")
                Text("pitch: \(prev.attitude.1)")
                Text("yaw: \(prev.attitude.2)")
            }
            .padding()
        }
        .onAppear {
            do { try client.connect(host, Int(port) ?? 0) }
            catch {
                err_desc = "\(error)"
                err = true
            }
            imu.preview { prev in
                self.prev = prev
                if (!client.send(prev)) {
                    err_desc = "disconnect"
                    err = true
                }
            }
        }
        .onDisappear {
            imu.preview { _ in }
            client.disconnect()
        }
        .gesture(
            DragGesture(minimumDistance: 0, coordinateSpace: .local)
                .onEnded { _ in
                    if (!client.reset()) {
                        err_desc = "disconnect"
                        err = true
                    }
                }
        )
        .alert(isPresented: $err) {
            presentationMode.wrappedValue.dismiss()
            return Alert(title: Text(err_desc))
        }
    }
}

struct ContentView: View {
    @State private var host: String = "192.168.11.3"
    @State private var port: String = "50051"
    var body: some View {
        NavigationView {
            VStack {
                TextField("IP address", text: $host)
                TextField("port", text: $port).keyboardType(.numberPad)
                NavigationLink(destination: MainView($host, $port)) {
                    Text("start").font(.title)
                }
                .navigationTitle("gRCP Server Setting")
            }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
