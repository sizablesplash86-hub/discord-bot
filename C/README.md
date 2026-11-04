Discord bot starter guide

Hello! SizableSplash (aka: Trevor) here. Thank you for downloading my Discord bot program. This code is intended to work with the Linux/POSIX operating systems. I designed this code on Debian, no guarentees to work on other distros. It should work, just be aware if it doesn't, you'll know why. I just haven't tested it... yet. Windows compatibility is what I'm more curious about. My bot can do other things, though. It can... ok, that's really about it. I designed this code to work like the JDS for Minecraft. Run it, it generates the config on startup. To create the background process, create the daemon in ```/etc/systemd/system/``` Run ```sudo nano /etc/systemd/system/discord-bot.service``` and paste the following:

[Unit]
Description=C Discord Bot Daemon
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/path/to/binary/here/
ExecStart=/path/to/binary/here/binary
Restart=on-failure
RestartSec=10

[Install]
WantedBy=multi-user.target
Just replace your actual root path. Do it somewhere like ```/etc/discord-bot/```. Run ```sudo systemctl daemon-reload && sudo systemctl enable discord-bot && sudo systemctl start discord-bot && sudo systemctl status discord-bot``` to start it. Thank you for using my code!
