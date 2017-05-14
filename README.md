# BF4_SERVER_MONITOR

********* v1.2 **********
- Fixed new bug where Battlelog doesn't always immediately return 'Rush' info at round start.
- Cleaned up code (removed unsued files, organized DEFINEs)
- Fixed bug w/ EEPROM default values integrity check (upon FW flash).
*----------------------*

********* v1.1 **********
- Fixed rank divide by 0 bug [WEB_JSON_PARSE.ino]
- Password protected wifi configuratino portal (password = "NWG")
- Display firmware version # on web page
- Add display contrast config setting to web page (adjustable 1-100%)
- Swapped order in which destroyed & remaining MCOMs are drawn (blown up are now first)
- Fixed stability issues with WIFI client (improved error handling)
- Automatically save/load config settings to/from EEPROM (serverID, contrast, refresh)
*----------------------*

********* v1.0 **********
- Initial Release
*----------------------*

Future Items:
[*] ADMIN TOOLS - SUSPICIOUS STATS ALERT
[BUG] proper contrast ratio conversion for web page
[*] Check web form refresh arg for overflow (or # digits)
[*] replace logo w/ instructions to connect to wifi manager 
[*] add wifi disconnected auto-restart ESP
[*] add Debug web page w/ SDK version, ESP SDK VERSION #, FREE HEAP, UPTIME, ETC.
[*] Graphics improvements:
	[*]	Icons for CTF/OB
	[*] Rush: highlight/outline current MCOM 
