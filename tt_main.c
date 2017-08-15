

int main(int argc, char **argv)
{
	init_config(argv[1]); //config file
	init_signal();
	
	pthread_create( pthread_handle_tty );
	pthread_create( pthread_handle_network );
	
	pthread_join(&tty_id);
	pthread_join(&net_id);
	
	return 0;
}