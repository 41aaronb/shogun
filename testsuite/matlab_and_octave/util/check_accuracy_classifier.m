function y = check_accuracy(accuracy, alphas, bias, sv, classified)
	printf("alphas: %e, bias: %e, sv: %e, classified %e <--- accuracy: %e\n", alphas, bias, sv, classified, accuracy);

	if alphas>accuracy
		y=false;
	elseif bias>accuracy
		y=false;
	elseif sv>accuracy
		y=false;
	elseif classified>accuracy
		y=false;
	else
		y=true;
	end
