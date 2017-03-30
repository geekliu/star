#
# Regular cron jobs for the itep-power-manager package
#
0 4	* * *	root	[ -x /usr/bin/itep-power-manager_maintenance ] && /usr/bin/itep-power-manager_maintenance
