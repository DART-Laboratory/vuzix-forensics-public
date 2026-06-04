The following details what events occurred during log collection/tracing. Most Perfetto folders have both a .pftrace and .systrace file, either contained in the folder itself or in a .zip file, along with its config file.

**Perfetto/**

- **PDFViewer1 & PDFViewer2/:** both folders include trace files ran while opening the PDFviewer on the Vuzix. In PDFViewer1, Process 3841 (hud.pdfviewer process) syncs with the SensorService 1443 for the amount of time the PDFviewer was active.
- **QRCode Scanner/:** scanned a QR code from an image, opening a website in the Vuzix browser
- **TeamViewerApp/:** started a remote call on TeamViewer to the Vuzix glasses and shared a pdf file over the call, which downloaded on the glasses.
