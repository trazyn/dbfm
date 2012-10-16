
Config(dbfm.conf)
======
	# Alsa driver short name "alsa" or "pulse"(make sure pulseaudio has installed)
	driver=pulse

	# Douban.fm channels, curl "http://www.douban.com/j/app/radio/channels" | jsonpp or cat $XDG_CONFIG_HOME/dbfm/channels
	channel=-3

	# Log file
	log=/tmp/dbfm.log

	# Error log
	err=/tmp/dbfm.log

	# Download directory
	download=/media/Music/download

------------------------------------------------------------------------

	A simple client for www.douban.com

	Github: www.github.com/tnrazy/dbfm


	Key map:
		
		n - Play next track

		s - Skip playlist

		l - Mark track as loved

		u - Unlove

		b - Banned current track

		d - Download current track (IDV3 tag undone)

		p - Print playlist

		i - Track info

		c - Change channel (Channel list undone, just for telnet)

		Q - Exit

		? - Print help

------------------------------------------------------------------------

