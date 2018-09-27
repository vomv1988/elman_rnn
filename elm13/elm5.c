#include <stdio.h>
#include <stdlib.h>

typedef struct srn{ /* Simple Recurrent Network */
	int
		inlyr,
		outlyr,
		**neurolayers,
		*neurolengs,
		layers,
		***weightlayers,
		weights,
		(*propagation)[2],
		fitness
	;
} Srn, *SrnPtr;

typedef struct sample{ /* Dataset sample */
	int
		*inputs,
		*outputs
	;
} Sample, *SamplePtr;

typedef struct dataset{ /* Whole dataset */
	SamplePtr samples; /* array of samples */
	int
		inlen, /* all samples must have the same amount of inputs */
		outlen, /* and outputs */
		datasetlen
	;
} Dataset, *DatasetPtr;

SrnPtr netalloc(
	int layers,
	int inlyr,
	int outlyr,
	int *neurolengs,
	int weights,
	int (*propagation)[2]
);
int netfree(SrnPtr net);
int randd(int x);
int clearnet(SrnPtr net);
int printnet(SrnPtr net);
int disturbnet(SrnPtr net);
int propanet(SrnPtr net, SamplePtr sample);
SamplePtr sampalloc(int *inputs, int inlen, int *outputs, int outlen);
int sampfree(SamplePtr sample);
int clearlayer(SrnPtr net, int layer);
DatasetPtr datalloc(int datasetlen, int inlen, int outlen);
int datfree(DatasetPtr data);
int cleardata(DatasetPtr data);
int copysample(DatasetPtr data, int dest, int ori);
int pushsample(DatasetPtr data, int *inputs, int *outputs);
int printdata(DatasetPtr data);
int greedytrain(SrnPtr net, DatasetPtr data, int epochs);
int neurooutput(int sum);
int copynet(SrnPtr dest, SrnPtr ori);
int propattnet(SrnPtr net, DatasetPtr data, int samples);

int main(int argc, char *argv[]){
	int
		ctxhid = atoi(argv[1]), /* number of context and hidden neurons */
		datalen = atoi(argv[2]), /* trainset length */
		greedyepochs = atoi(argv[3]) /* epochs for greedy trainer */
	;

	int layers = 4; /* number of neurolayers in srn */
	int weights = 3; /* number of weightlayers in srn */
	int neurolengs[] = { /* neurolayer lengths */
		1, /* 0 input */
		ctxhid, /* 1 hidden */
		ctxhid, /* 2 context */
		1 /* 3 output */
	};
	int inlyr = 0; /* standardize where input and output layers are instead of this*/
	int outlyr = 3;
	int propagation[][2] = { /* propagation pattern for srn */
		{2, 1}, /* connect neurolayer 2 with 1 through weightlayer 0 */
		{0, 1}, /* etc. */
		{1, 3} /* etc. */
	};

	SrnPtr net = netalloc(layers, inlyr, outlyr, neurolengs, weights, propagation);
	clearnet(net);

	int datasetlen = datalen;
	int inlen = net->neurolengs[net->inlyr];
	int outlen = net->neurolengs[net->outlyr];

	DatasetPtr data = datalloc(datasetlen, inlen, outlen);

	int i;
	for(i = 0; i < data->datasetlen - 1; i ++)
		readsample(data);

	int j;
	for(i = 1; !readsample(data); i ++)
		for(j = 1; j <= 31; j ++){
			printf("%d.%d. ", i, j);
			greedytrain(net, data, greedyepochs);
		}

	netfree(net);
	datfree(data);

	return 0;
}

SrnPtr netalloc(
	int layers,
	int inlyr,
	int outlyr,
	int *neurolengs,
	int weights,
	int (*propagation)[2]
){
	int i, j;

	int **neurolayers = (int **) malloc(sizeof(int *) * layers);
	for(i = 0; i < layers; i ++)
		neurolayers[i] = (int *) malloc(sizeof(int) * neurolengs[i]);

	int ***weightlayers = (int ***) malloc(sizeof(int **) * weights);
	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < weights; i ++){
		from = propagation[i][0];
		fromlen = neurolengs[from];

		to = propagation[i][1];
		tolen = neurolengs[to];

		weightlayers[i] = (int **) malloc(sizeof(int *) * fromlen);
		for(j = 0; j < fromlen; j ++)
			weightlayers[i][j] = (int *) malloc(sizeof(int) * tolen);
	}

	SrnPtr net = (SrnPtr) malloc(sizeof(Srn));

	net->layers = layers;
	net->inlyr = inlyr;
	net->outlyr = outlyr;
	net->weights = weights;
	net->neurolengs = neurolengs;
	net->propagation = propagation;

	net->neurolayers = neurolayers;
	net->weightlayers = weightlayers;

	return net;
}

int netfree(SrnPtr net){
	int i, j;

	for(i = 0; i < net->layers; i ++)
		free(net->neurolayers[i]);
	free(net->neurolayers);

	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			free(net->weightlayers[i][j]);
		free(net->weightlayers[i]);
	}
	free(net->weightlayers);

	free(net);

	return 0;
}

int randd(int x){
	return x + rand() % 3 - 1;
}

int clearnet(SrnPtr net){
	int i, j, k;

	for(i = 0; i < net->layers; i ++)
		for(j = 0; j < net->neurolengs[i]; j ++)
			net->neurolayers[i][j] = 0;
	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				net->weightlayers[i][j][k] = 0;
	}

	return 0;
}

int printnet(SrnPtr net){
	int i, j, k;

	printf("layers: %d,  weights: %d\n", net->layers, net->weights);

	for(i = 0; i < net->layers; i ++)
		printf("neuroleng %d: %d\n", i, net->neurolengs[i]);

	for(i = 0; i < net->layers; i ++)
		for(j = 0; j < net->neurolengs[i]; j ++)
			printf("neuron %d, %d: %d\n", i, j, net->neurolayers[i][j]);
	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		printf("weightlayer %d: from neurolayer %d to neurolayer %d\n", i, from, to);

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				printf("weight %d, %d: %d\n", j, k, net->weightlayers[i][j][k]);
	}

	return 0;
}

int disturbnet(SrnPtr net){
	int i, j, k;

	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				net->weightlayers[i][j][k] = randd(net->weightlayers[i][j][k]);
	}

	return 0;
}

int propanet(SrnPtr net, SamplePtr sample){
	int i, j, k;

	for(i = 0; i < net->neurolengs[net->inlyr]; i ++)
		net->neurolayers[net->inlyr][i] = sample->inputs[i];

	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < net->weights; i ++){
		from = net->propagation[i][0];
		fromlen = net->neurolengs[from];

		to = net->propagation[i][1];
		tolen = net->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				net->neurolayers[to][k] +=
					neurooutput(net->neurolayers[from][j]) *
					net->weightlayers[i][j][k]
				;
	}

	/* copy hidden to context */
	for(i = 0; i < net->neurolengs[1]; i ++)
		net->neurolayers[2][i] = net->neurolayers[1][i];

	int error = 0;
	for(i = 0; i < net->neurolengs[net->outlyr]; i ++){
		error += neurooutput(net->neurolayers[net->outlyr][i]) - sample->outputs[i];
		if(error < 0)
			error *= -1;
	}

	return error;
}

SamplePtr sampalloc(int *inputs, int inlen, int *outputs, int outlen){
	int i;
	SamplePtr sample = (SamplePtr) malloc(sizeof(Sample));

	sample->inputs = (int *) malloc(sizeof(int) * inlen);
	for(i = 0; i < inlen; i ++)
		sample->inputs[i] = inputs[i];

	sample->outputs = (int *) malloc(sizeof(int) * outlen);
	for(i = 0; i < outlen; i ++)
		sample->outputs[i] = outputs[i];

	return sample;
}

int sampfree(SamplePtr sample){
	free(sample->inputs);
	free(sample->outputs);
	free(sample);

	return 0;
}

int clearlayer(SrnPtr net, int layer){
	int i;

	for(i = 0; i < net->neurolengs[layer]; i ++)
		net->neurolayers[layer][i] = 0;

	return 0;
}

DatasetPtr datalloc(int datasetlen, int inlen, int outlen){
	int i;
	DatasetPtr data = (DatasetPtr) malloc(sizeof(Dataset));
	data->samples = (SamplePtr) malloc(sizeof(Sample) * datasetlen);

	for(i = 0; i < datasetlen; i ++){
		data->samples[i].inputs = (int *) malloc(sizeof(int) * inlen);
		data->samples[i].outputs = (int *) malloc(sizeof(int) * outlen);
	}

	data->inlen = inlen;
	data->outlen = outlen;
	data->datasetlen = datasetlen;

	return data;
}

int datfree(DatasetPtr data){
	int i;

	for(i = 0; i < data->datasetlen; i ++){
		free(data->samples[i].inputs);
		free(data->samples[i].outputs);
	}

	free(data->samples);
	free(data);

	return 0;
}

int cleardata(DatasetPtr data){
	int i, j;

	for(i = 0; i < data->datasetlen; i ++){
		for(j = 0; j < data->inlen; j ++)
			data->samples[i].inputs[j] = 0;
		for(j = 0; j < data->outlen; j ++)
			data->samples[i].outputs[j] = 0;
	}

	return 0;
}

int copysample(DatasetPtr data, int dest, int ori){
	int i;
	for(i = 0; i < data->inlen; i ++)
		data->samples[dest].inputs[i] = data->samples[ori].inputs[i];
	for(i = 0; i < data->outlen; i ++)
		data->samples[dest].outputs[i] = data->samples[ori].outputs[i];
	return 0;
}

int pushsample(DatasetPtr data, int *inputs, int *outputs){
	/* suboptimal, but the gain from using lists would be marginal */
	int i;
	for(i = 1; i < data->datasetlen; i ++)
		copysample(data, i - 1, i);

	for(i = 0; i < data->inlen; i ++)
		data->samples[data->datasetlen - 1].inputs[i] = inputs[i];
	for(i = 0; i < data->outlen; i ++)
		data->samples[data->datasetlen - 1].outputs[i] = outputs[i];

	return 0;
}

int readsample(DatasetPtr data){
	int i;
	int *inputs = (int *) malloc(sizeof(int) * data->inlen);
	int *outputs = (int *) malloc(sizeof(int) * data->outlen);

	for(i = 0; i < data->inlen; i ++)
		inputs[i] = 0;
	for(i = 0; i < data->outlen; i ++)
		outputs[i] = 0;

	for(i = 0; i < data->inlen; i ++)
		if(scanf("%d", inputs + i) == EOF){
			pushsample(data, inputs, outputs);
			free(inputs);
			free(outputs);
			return 1;
		}
	for(i = 0; i < data->outlen; i ++)
		if(scanf("%d", outputs + i) == EOF){
			pushsample(data, inputs, outputs);
			free(inputs);
			free(outputs);
			return 1;
		}

	pushsample(data, inputs, outputs);
	free(inputs);
	free(outputs);

	return 0;
}

int printdata(DatasetPtr data){
	int i, j;
	printf("Inputs %d, outputs %d\n", data->inlen, data->outlen);

	for(i = 0; i < data->datasetlen; i ++){
		printf("Sample %d:\n", i);
		for(j = 0; j < data->inlen; j ++)
			printf("  Input %d: %d\n", j, data->samples[i].inputs[j]);
		for(j = 0; j < data->outlen; j ++)
			printf("  Output %d: %d\n", j, data->samples[i].outputs[j]);
	}

	return 0;
}

int greedytrain(SrnPtr net, DatasetPtr data, int epochs){
	int i, j, minfit;

	SrnPtr best = netalloc(
		net->layers,
		net->inlyr,
		net->outlyr,
		net->neurolengs,
		net->weights,
		net->propagation
	);

	clearnet(best);
	disturbnet(best);
	minfit = propattnet(best, data, data->datasetlen - 1);
	copynet(net, best);

	for(i = 0; i < epochs && minfit > 0; i ++){
		disturbnet(net);
		propattnet(net, data, data->datasetlen - 1);
		if(net->fitness < minfit){
			copynet(best, net);
			minfit = net->fitness;
		} else
			copynet(net, best);
	}

	propattnet(best, data, data->datasetlen);
	printf(
		"act %d, pred %d, sxs %d, qlty %d\n",
		data->samples[data->datasetlen - 1].outputs[0],
		neurooutput(best->neurolayers[3][0]),
		data->samples[data->datasetlen - 1].outputs[0] == neurooutput(best->neurolayers[3][0]),
		minfit, i
	);

	fflush(NULL);

	copynet(net, best);
	netfree(best);
	return 0;
}

int neurooutput(int sum){
	if(sum > 0)
		return 1;
	else
		return -1;
}

int copynet(SrnPtr dest, SrnPtr ori){
	int i, j, k;

	for(i = 0; i < dest->layers; i ++)
		for(j = 0; j < dest->neurolengs[i]; j ++)
			dest->neurolayers[i][j] = ori->neurolayers[i][j];
	int
		from, fromlen,
		to, tolen
	;
	for(i = 0; i < dest->weights; i ++){
		from = dest->propagation[i][0];
		fromlen = dest->neurolengs[from];

		to = dest->propagation[i][1];
		tolen = dest->neurolengs[to];

		for(j = 0; j < fromlen; j ++)
			for(k = 0; k < tolen; k ++)
				dest->weightlayers[i][j][k] = ori->weightlayers[i][j][k];
	}
	return 0;
}

int propattnet(SrnPtr net, DatasetPtr data, int samples){
	int i, error = 0;

	clearlayer(net, 0); /* group context layers to avoid this */
	clearlayer(net, 1);
	clearlayer(net, 2);
	clearlayer(net, 3);

	for(i = 0; i < samples; i ++){
		clearlayer(net, 0); /* group context layers to avoid this */
		clearlayer(net, 1);
		clearlayer(net, 3);
		error += propanet(net, &data->samples[i]);
	}

	net->fitness = error;
	return error;
}
