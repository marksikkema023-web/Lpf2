# LEGO BLE Packet Capture & Parse Automation Script (Press Enter to Stop)

Write-Host "[1/3] Starting Bluetooth Packet Capture..." -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Yellow
Write-Host "Capturing active data... Connect your Hub and do your actions now." -ForegroundColor White
Write-Host "---> PRESS [ENTER] IN THIS WINDOW TO STOP CAPTURING <---" -ForegroundColor Yellow
Write-Host "==========================================================" -ForegroundColor Yellow

# Start the logger silently in the background
$CaptureProcess = Start-Process -FilePath ".\idevicebtlogger.exe" -ArgumentList "-f pcap output.pcap" -NoNewWindow -PassThru

# Wait right here until the user presses the ENTER key
Read-Host

Write-Host "`nStopping packet capture safely..." -ForegroundColor Cyan
# Kill ONLY the logger process, preserving the PowerShell script execution context
Stop-Process -Id $CaptureProcess.Id -Force
Start-Sleep -Seconds 1

Write-Host "`n[2/3] Extracting Bluetooth ATT fields via TShark..." -ForegroundColor Cyan
& "C:\Program Files\Wireshark\tshark.exe" -r .\output.pcap -Y "btatt" -T fields -e frame.time_relative -e btatt.opcode -e btatt.value | Out-File -FilePath .\clean_lego_log.txt -Encoding unicode

Write-Host "[3/3] Running Python parser..." -ForegroundColor Cyan
if (Test-Path .\clean_lego_log.txt) {
    python parse_lego.py
}

# --- TIME STAMP ARCHIVING BLOCK ---
$TimeStamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"

if (Test-Path .\parsed_results.txt) {
    $UniqueResultsName = "parsed_results_$TimeStamp.txt"
    Rename-Item -Path .\parsed_results.txt -NewName $UniqueResultsName
    Write-Host "`n[SUCCESS] Pipeline complete!" -ForegroundColor Green
    Write-Host "Your history entry has been saved as: $UniqueResultsName" -ForegroundColor Yellow
    
    # Opens the file in Notepad immediately
    notepad.exe .\$UniqueResultsName
} else {
    Write-Host "`n[ERROR] Python script failed to generate data. Check if clean_lego_log.txt contains fields." -ForegroundColor Red
}