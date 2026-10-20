Discord bot starter guide

Hello! SizableSplash (aka: Trevor) here. Thank you for downloading my Discord bot program. This code is primarily intended to work as a status checker for a home server. My bot can do other things, though. It can... ok, that's really about it. I designed this code to work like the JDS for Minecraft. Run it, it generates the config on startup. To create the background process, create the daemon in ```/etc/systemd/system/``` Run ```sudo nano /etc/systemd/system/discord-bot.service``` and paste the following:

[Unit]
Description=Discord bot Lorem Ipsum code
After=network.target

[Service]
Type=simple
User=root
ExecStart=/path/to/binary/here/lorem-ipsum
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target

Just replace your actual root path. ```sudo systemctl daemon-reload && sudo systemctl enable discord-bot && sudo systemctl start discord-bot && sudo systemctl status discord-bot``` to run it all and check the status. Thank you for using my code!
