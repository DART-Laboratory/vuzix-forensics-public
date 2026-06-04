## Overview of Findings from Vuzix M4000 Log Collection and Trace Analysis

**Repository Structure:**

**Dumps/** (Initial log dumps collected with `adb logcat -d`)

**Logcat/**
- Youtube/
- BrowserDownload/
- Idle/
- Camera App/ 

**Perfetto/**
- PDFViewer1/
- PDFViewer2/
- QRCode Scanner/
- TeamViewerApp/ 

### Initial Log Collection and Environment Setup
The first stage of the project focused on setting up logging tools and understanding how much system information could be collected from the Vuzix M4000 smart glasses. Using Android Debug Bridge (ADB), multiple `logcat` sessions were collected under both idle and active device conditions. Initial tests included:
- Full log dumps using `adb logcat -d`
- Continuous logging sessions with `adb logcat -f /sdcard/vuzix_log.txt -r 50 -n 5`
- Activity-based logging while navigating system settings, camera applications, and browser applications

### Browser and PDF Viewer
#### Browser and Network Activity
The built-in Vuzix browser can be used to generate network activity. Tests included:
- Opening YouTube
- Visiting external websites
- Downloading PDF files online (from the DartLab website)
- Opening downloaded PDFs within the Vuzix PDF viewer

During these sessions, logcat revealed activity associated with Android WebView processes through WebViewShell messages. For example, this event:
`03-14 18:32:31.972  3596  3596 E WebViewShell: https://dartlab.org/assets/pdf/mirage.pdf`
(Logcat / BrowserDownload / browser_logs.txt.03) shows that browser navigation and download-related network activity could be partially reconstructed from system logs, though the actual file transfer process itself was not directly exposed through logcat.

#### PDF Viewer and Head Tracking
The Vuzix built-in PDF viewer supports head-tracking navigation. While moving the headset, users can scroll and navigate documents using head movement, making the built-in pdfviewer an easy tool to test HMD logging.

Head-tracking and HMD sensor information does not appear in standard `logcat` output, however, sensor-related information is observable through `adb shell dumpsys sensorservice`.

The sensor service dump exposes active sensors, sensor clients and processes, recent sensor events, permissions and subscriptions, and apps interacting with sensor services


### Perfetto Tracing and Process Interaction Analysis
#### Setting Up Perfetto
The goal for perfetto was to collect lower-level execution traces (syscalls & scheduling) to analyze relationships between applications and system services. Attempting to run Perfetto through the command line resulted in repeated permission issues when writing trace files to device directories (like `/data/misc/perfetto-traces/` and `/sdcard/`). Traces were instead collected using the online Perfetto UI and gui configuration.

Furthermore, the Vuzix M4000 kernel blocks many low-level tracing sources; when queried,
- `raw_syscall` returned zero events
- Filesystem trace categories (`ext4/*, f2fs/*`) returned no data

So only scheduling (`sched_wakeup`, `sched_switch`) and task events could be reliably traced with Perfetto.

#### Process Relationships and Sensor Interactions
When tracing the PDF viewer application while head tracking was active, Perfetto traces showed:
- The `Vuzix.hud.pdfviewer` process syncing with Android `SensorService`
- Consistent wakeup and scheduling relationships between the PDF viewer and sensor-related services

The PDF viewer and sensor service repeatedly appeared together in scheduling relationships, so continuous communication between the application and the sensor subsystem during head-tracked viewing can be inferred.
#### Activity Tracing
Additional Perfetto traces were collected during:
- QR code scanning
- Browser navigation
- Remote TeamViewer sessions (3rd Party Remote Assist App)
- File transfers during remote sessions (on Team Viewer)

However, due to kernel restrictions, only scheduling behavior was visible. Direct syscall tracing and filesystem-level events remained inaccessible.

#### Downloading 3rd-Party Apps
Third party apps, such as remote-assist apps, integrate real time streaming and file-sharing with Vuzix-specific features like AR overlays, cameras, and head-tracking sensors. They are also the apps Vuzix users likely interact the most with, which make them ideal for collecting relevant logs. 
Apps can be downloaded from the Vuzix app store online once logged in with the credentials for the Vuzix glasses.
	
### Conclusions Regarding Logging:
1. **Standard ADB Logging Limitations**:
- Logcat is useful for browser navigation, app lifecycle events, WebView activity, and general system behavior. However, it does not expose HMD tracking data or Sensor HAL interactions. These require sensor service inspection or scheduler-level tracing.
2. **Android System Services**
- The SensorService subsystem is central to head tracking, and sensor client communication. Applications like the Vuzix PDF viewer interact with this subsystem continuously during active head tracking.
3. **Kernel Restrictions Limit Trace Visibility**
- The Vuzix M4000 kernel blocks tracing mechanisms such as raw syscalls, filesystem events, and traditional systrace functionality. This limits causal analysis primarily to scheduling relationships rather than full syscall or file-access tracing.
- Even without syscalls, Perfetto scheduling traces can still reveal which processes interact frequently, wakeup dependencies between services, potential causal relationships between applications and sensor services.

### Future Work / Potential Next Steps
- Standardizing Perfetto configurations across all traces
- Recollecting traces using only supported scheduling events
- Building Python scripts to parse pftrace/systrace data automatically
- Extracting thread relationships from specific applications
- Comparing Vuzix traces with traces from the RealityCheck Meta Quest 2 repository
- Investigating whether provenance graphs can be reconstructed from scheduler-level data alone
- Exploring alternative tracing tools such as `atrace`
- Applying automated/LLM analysis to identify behavioral patterns in traces
