Scriptname WikiMapIntegration_MCM extends MCM_ConfigBase

Event OnSettingChange(string id)
    if id == "iModifierKeyCode:General"
        int k = GetModSettingInt("iModifierKeyCode:General")
        if k != 184 && k != 56 && k != 157 && k != 29 && k!= 42 && k !=54
            SetModSettingInt("iModifierKeyCode:General", -1)
        endIf
        ;; RightAlt = 184
        ;; LeftAlt = 56
        ;; RightControl = 157
        ;; LeftControl = 29
        ;; LeftShift = 42
        ;; RightShift = 54
        self.RefreshMenu()
    endIf
    if id == "iKeyCode:General"
        int k = GetModSettingInt("iKeyCode:General")
        ;;L=38, J=36, E=18, F=33, M=50, ESC=1
        if k == 38 || k == 36 || k == 18 || k == 33 || k == 1 || k == 50
            SetModSettingInt("iKeyCode:General", 53)
        endIf
        self.RefreshMenu()
    endIf
EndEvent

function OnConfigClose() native
