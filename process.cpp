void process(){
	enemy enm;
	enm.init(10.0f, RED, 0, 0, 600, 0);

	for(int i = 0; i < 10000; i++){
		for(int j = 0; j < 5; j++){
			enm.createSimple(3.5f, YELLOW, 0.0f, 0.0f, i * i / 2600.0f + j * 0.4f * PI - 0.5f * PI, 4.0f);
		}
		wait(1);
	}

	wait(180);
}
