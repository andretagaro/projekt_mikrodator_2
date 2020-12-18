








ISR(USART0_RX_vect)
{
	data_ascii = UDR0;

	if (data_ascii != 10) {
		test_data[i] = data_ascii;
		i++;
	}else if (data_ascii == 10) {
		received = (test_data[0] - 48);
		received0 = (test_data[1] - 48);
		received1 = (test_data[2] - 48);
		 i = 0;
	}
}
