/*
 * Copyright (c) 2014, Shogun Toolbox Foundation
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:

 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its 
 * contributors may be used to endorse or promote products derived from this 
 * software without specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * Written (W) 2014 Khaled Nasr
 */

#ifndef __NEURALNETWORK_H__
#define __NEURALNETWORK_H__

#include <shogun/lib/common.h>
#include <shogun/machine/Machine.h>
#include <shogun/lib/SGVector.h>
#include <shogun/features/DenseFeatures.h>
#include <shogun/lib/DynamicObjectArray.h>
#include <shogun/neuralnets/NeuralLayer.h>

namespace shogun
{
/** @brief A generic multi-layer neural network
 *
 * NeuralNetwork is constructed using an array of NeuralLayer objects. The
 * NeuralLayer class defines the interface necessary for forward and 
 * backpropagation.
 * 
 * The network takes as input CDenseFeatures<float64_t> and outputs
 * CMulticlassLabels.
 * 
 * Computations are performed using Eigen3
 * 
 * The network stores the parameters (and parameter gradients) of all the 
 * layers in a single array. This makes it easy to train a network of any
 * combination of arbitrary layer types using any optimization method (gradient
 * descent, L-BFGS, ..)
 * 
 * All the matrices the network (and related classes) deal with are in 
 * column-major format
 * 
 * When implemnting new layer types, the function check_gradients() can be used
 * to make sure the gradient computations are correct.
 */
class CNeuralNetwork : public CMachine
{
public:
	/** default constuctor */
	CNeuralNetwork();
	
	/** Initializes the network
	 * 
	 * @param num_inputs number of inputs the network takes
	 * 
	 * @param layers An array of NeuralLayer objects specifying the hidden 
	 * and output layers in the network.
	 */
	virtual void initialize(int32_t num_inputs, CDynamicObjectArray* layers);
	
	virtual ~CNeuralNetwork();
	
	/** apply machine to data in means of binary classification problem */
	virtual CBinaryLabels* apply_binary(CFeatures* data);
	/** apply machine to data in means of regression problem */
	virtual CRegressionLabels* apply_regression(CFeatures* data);
	/** apply machine to data in means of multiclass classification problem */
	virtual CMulticlassLabels* apply_multiclass(CFeatures* data);
	
	/** set labels
	*
	* @param lab labels
	*/
	virtual void set_labels(CLabels* lab);
	
	/** get classifier type
	 *
	 * @return classifier type CT_NEURALNETWORK
	 */
	virtual EMachineType get_classifier_type() {return CT_NEURALNETWORK;}
	
	/** returns type of problem machine solves */
	virtual EProblemType get_machine_problem_type() const;
	
	/** Checks if the gradients computed using backpropagation are correct by 
	 * comparing them with gradients computed using numerical approximation.
	 * Used for testing purposes only.
	 * 
	 * @param epsilon constant used during gradient approximation
	 * 
	 * @param tolerance maximum difference allowed between backpropagation 
	 * gradients and numerical approximation gradients
	 * 
	 * @return true if the gradients are correct, false otherwise
	 */
	virtual bool check_gradients(float64_t epsilon=1.0e-06, 
			float64_t tolerance=1.0e-09);
	
	/** returns the totat number of parameters in the network */
	int32_t get_num_parameters() {return m_total_num_parameters;}
	
	/** returns a pointer to the network's parameter array */
	float64_t* get_parameters() {return m_params;}
	
	/** returns a pointer to the network's parameter gradients array */
	float64_t* get_parameter_gradients() {return m_param_gradients;}
	
	/** returns the number of inputs the network takes*/
	int32_t get_num_inputs() {return m_num_inputs;}
	
	/** returns the number of neurons in the output layer */
	int32_t get_num_outputs()
	{
		return get_layer(m_num_layers-1)->get_num_neurons();
	}
	
	virtual const char* get_name() const { return "NeuralNetwork";}
	
protected:	
	/** trains the network using gradient descent */
	virtual bool train_machine(CFeatures* data=NULL);
	
	/** Applies forward propagation, computes the activations of each layer
	 * 
	 * @param data input features
	 * @return pointer to the activations of the last layer
	 */
	virtual float64_t* forward_propagate(CFeatures* data);
	
	/** Applies forward propagation, computes the activations of each layer
	 * 
	 * @param inputs inputs to the network, a matrix of size 
	 * m_input_layer_num_neurons*m_batch_size
	 * 
	 * @return pointer to the activations of the last layer
	 */
	virtual float64_t* forward_propagate(float64_t* inputs);
	
	/** Sets the batch size (the number of train/test cases) the network is 
	 * expected to deal with. 
	 * Allocates memory for the activations, local gradients, input gradients
	 * if necessary (if the batch size is different from it's previous value)
	 * 
	 * @param batch_size number of train/test cases the network is expected to 
	 * deal with.
	 */
	virtual void set_batch_size(int32_t batch_size);
	
	/** Applies backpropagation to compute the gradients of the error with
	 * repsect to every parameter in the network. Results are stored in 
	 * m_param_gradients and can be accessed by calling 
	 * get_parameter_gradients()
	 *
	 * @param inputs inputs to the network, a matrix of size 
	 * m_input_layer_num_neurons*m_batch_size
	 * 
	 * @param targets desired values for the output layer's activations. matrix 
	 * of size m_layers[m_num_layers-1].get_num_neurons()*m_batch_size
	 */
	virtual void compute_gradients(float64_t* inputs, float64_t* targets);
	
	/** Computes the error between the output layer's activations and the given
	 * target activations.
	 *
	 * Regularization error is ignored.
	 * 
	 * @param targets desired values for the network's output, matrix of size
	 * num_neurons_output_layer*batch_size
	 * 
	 * @param inputs if NULL, the error is computed between the current 
	 * activations and the given targets, no forward propagation is performed.
	 * otherwise [if inputs is a matrix of size 
	 * input_layer_num_neurons*batch_size], forward propagation is performed to
	 * update the activations before the error is computed.
	 */
	virtual float64_t compute_error(float64_t* targets, 
			float64_t* inputs=NULL);
	
	virtual bool is_label_valid(CLabels *lab) const;
	
private:
	/** returns a pointer to layer i in the network */
	CNeuralLayer* get_layer(int32_t i)
	{
		CNeuralLayer* layer = (CNeuralLayer*)m_layers->element(i);
		SG_UNREF(layer);
		return layer;
	}
	
	/** returns a pointer to the portion of m_params that belongs to layer i */
	float64_t* get_layer_params(int32_t i)
	{
		return m_params.vector+m_index_offsets[i];
	}
	
	/** returns a pointer to the portion of m_param_gradients that belongs to
	 * layer i
	 */
	float64_t* get_layer_param_gradients(int32_t i)
	{
		return m_param_gradients.vector+m_index_offsets[i];
	}
	
	/** returns a pointer to the portion of m_param_regularizable that belongs
	 * to layer i
	 */
	bool* get_layer_param_regularizable(int32_t i)
	{
		return m_param_regularizable.vector+m_index_offsets[i];
	}
	
	/** returns a pointer to the raw data of the given features */
	float64_t* features_to_raw(CFeatures* features);
	
	/** converts the given labels into an array suitable for use with network
	 * 
	 * @return newly allocated matrix of size get_num_outputs()*num_labels
	 */
	float64_t* labels_to_raw(CLabels* labs);
	
	void init();
	
public:
	/** L2 Regularization coeff, default value is 0.0*/
	float64_t l2_coefficient;
	
	/** size of the mini-batch used during training, if 0 full-batch training is
	 * performed
	 */
	int32_t mini_batch_size;
	
	/** training parameters, maximum number of iterations over the training set
	 * defualt value is 100
	 */
	int32_t max_num_epochs;
	
	/** gradient descent learning rate, defualt value 0.1 */
	float64_t gd_learning_rate;
	
	/** gradient descent momentum multiplier, default value 0.9 */
	float64_t gd_momentum;
	
protected:
	/** number of neurons in the input layer */
	int32_t m_num_inputs;
	
	/** number of layer */
	int32_t m_num_layers;
	
	/** network's layers */
	CDynamicObjectArray* m_layers;
	
	/** total number of parameters in the network */
	int32_t m_total_num_parameters;
	
	/** array where all the parameters of the network are stored */
	SGVector<float64_t> m_params;
	
	/** array where the gradients of the error with respect to the parameters 
	 * are stored
	 */
	SGVector<float64_t> m_param_gradients;
	
	/** Array that specifies which parameters are to be regularized. This is 
	 * used to turn off regularization for bias parameters
	 */
	SGVector<bool> m_param_regularizable;
	
	/** offsets specifying where each layer's parameters and parameter 
	 * gradients are stored, i.e layer i's parameters are stored at 
	 * m_params + m_index_offsets[i]
	 */
	SGVector<int32_t> m_index_offsets;
	
	/** number of train/test cases the network is expected to deal with.
	 * defaul value is 1
	 */
	int32_t m_batch_size;
};
	
}
#endif
