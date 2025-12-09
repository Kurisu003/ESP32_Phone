; Run immediately on script startup
RunPIA()
ExitApp   ; terminate this script after running once

moveMouse(x, y) {
    DllCall("SetCursorPos", "int", x, "int", y)
}

RunPIA() {
    Send "{LWin}"
    Sleep 150
    Send "private internet access"
    Sleep 150
    Send "{Enter}"
    Sleep 500

    x := 3300
    y := 1313
    moveMouse(x, y)
    Sleep 150
    Click
    Sleep 150

    moveMouse(3440 / 2, 1440 / 2)
    Sleep 150
    Click
    Sleep 150
    Click
    Sleep 150

    Send "{Ctrl Down}{End}{Ctrl Up}"
    Sleep 150

    Send "{Up}"
    Send "{Up}"
    Send "{Up}"
    Sleep 150

    Send "{End}"
    Sleep 150

    Send "{Left}"
    Sleep 150

    Send "{Ctrl Down}{BS}{Ctrl Up}"
    Sleep 150

    Send "{Ctrl Down}v{Ctrl Up}"
    Sleep 150

    KillAllAHK()
}

KillAllAHK() {
    cmd := "taskkill /F /IM AutoHotkey.exe"
    Run cmd, , "Hide"
}
