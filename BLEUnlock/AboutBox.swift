import Cocoa

private var aboutBox: AboutBox? = nil

class AboutBox: NSWindowController, NSWindowDelegate {
    @IBOutlet weak var versionLabel: NSTextField!
    private var didNotifyOpen = false

    @IBAction func visitHomepage(_ sender: Any) {
        NSWorkspace.shared.open(URL(string: "https://github.com/Skyearn/BLEUnlock#readme")!)
    }

    @IBAction func checkReleases(_ sender: Any) {
        NSWorkspace.shared.open(URL(string: "https://github.com/Skyearn/BLEUnlock/blob/master/CHANGELOG.md")!)
    }
    convenience init() {
        self.init(windowNibName: "AboutBox")
    }

    override func windowDidLoad() {
        super.windowDidLoad()
        window?.delegate = self
        if let info = Bundle.main.infoDictionary {
            if let version = info["CFBundleShortVersionString"] as? String {
                if let build = info["CFBundleVersion"] as? String {
                    versionLabel.stringValue = versionLabel.stringValue.replacingOccurrences(of: "#{version}", with: "\(version) (\(build))")
                }
            }
        }
    }

    override func cancelOperation(_ sender: Any?) {
        close()
    }

    func windowWillClose(_ notification: Notification) {
        if didNotifyOpen {
            didNotifyOpen = false
            (NSApp.delegate as? AppDelegate)?.aboutBoxWillClose()
        }
    }

    static func showAboutBox() {
        if (aboutBox == nil) {
            aboutBox = AboutBox()
        }
        guard let box = aboutBox else { return }
        if !box.didNotifyOpen {
            box.didNotifyOpen = true
            (NSApp.delegate as? AppDelegate)?.aboutBoxDidOpen()
        }
        box.showWindow(nil)
        box.window?.orderFront(self)
    }
}
