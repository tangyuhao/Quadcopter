short autoTakeoff(unsigned short height,unsigned short step, unsigned short throttle_max )
{
	int i,chan = 1100;
	int motor12,motor13,motor14,motor23,motor24,motor34;
	write2mavproxy_rc(1,1500);
	msleep(50);
	write2mavproxy_rc(2,1500);
	msleep(50);
	write2mavproxy_rc(4,1500);	
	msleep(50);
	char motor_right = 0;
	short danger = 0;
	int ret;
	int status_len = sizeof(status);
	struct status_struct * sta = &status.info;
	for (i=1;chan < throttle_max;i++)
	{
		if (sta->xacc > 300 || sta->xacc <-300 || sta->yacc > 300 || sta->yacc <-300 )
		{
			danger = 1;
			if (sta->hud_alt > 0.6)
				write2mavproxy_mode(LAND);
			else 
				write2mavproxy_rc(3,1100);
			return -1;
		}
		chan += step;	
		write2mavproxy_rc(3,chan);
		msleep(100);
		write2mavproxy_status(sta);
		ret = wrap_send(client_sockfd_send, &status, status_len, 0);
					if(ret == -1)
					{
						perror("wrap_send error");
						return -1;
					}
					state_flag = RECV_HEADER;
	}
	write2mavproxy_mode(LOITER);
	sleep(5);
	write2mavproxy_rc(3,1100);
	return 0;
}
