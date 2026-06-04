The following details what events occurred during log collection/tracing. 

**Logcat/**

- **Youtube/:** searched "youtube.com" using Vuzix browser app. This can be seen in the logs with events that have "chromium.webview_shell"
- **BrowserDownload/:** searched the dart lab website on the Vuzix browser, downloaded mirage.pdf, opened mirage.pdf through downloads, opening a native PDFviewer app. The PDFviewer app uses headtracking, which does not show in logcat, but shows in sensorservice dump (“sensor_dump.txt”).
- **Idle/:** scrolled through system settings, adjusted brightness, then left glasses idle
- **Camera App/:** opened and used the camera app, captured a picture
