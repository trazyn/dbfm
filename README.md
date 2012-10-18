A simple client for www.douban.fm

Github: www.github.com/tnrazy/dbfm

------------------------------------------------------------------------

![preview](https://raw.github.com/tnrazy/dbfm/master/preview.png)

------------------------------------------------------------------------

Copy conf/* to $XDG_CONFIG_HOME/dbfm

Config

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
	
	port=7000

Key map:
#
	k - Move curosr up
	j - Move curosr down


	n - Play next track
	s - Skip playlist
	l - Mark track as loved
	u - Unlove
	b - Banned current track
	d - Download current track(ID3V2 tag undone)
	p - Print playlist
	i - Track info
	Q - Exit
	? - Print help
#

Run DBFM as daemon
#
	$ dbfm -d
	
	telnet localhost 7000
#

Commands:(response undone)
#
	next		: Play next
	skip 		: ...
	love 		: ...
	unlove 		: ...
	ban		: ...
	channel x 	: Change channel
	kill 		: Exit dbfm
#
