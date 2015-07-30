/*
 * Copyright (c) The Shogun Machine Learning Toolbox
 * Written (w) 2015 Wu Lin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the Shogun Development Team.
 *
 */

#ifndef FIRSTORDERMINIMIZER_H
#define FIRSTORDERMINIMIZER_H
#include <shogun/optimization/FirstOrderCostFunction.h>
#include <shogun/optimization/MinimizerContext.h>
namespace shogun
{

/** @brief The first order minimizer base class.
 *
 * This class gives the interface of a minimizer
 *
 */
class CFirstOrderMinimizer
{
public: 
	/** Default constructor */
	CFirstOrderMinimizer()
	{
		init();
	}
	/** Constructor
	 * @param fun cost function
	 */
	CFirstOrderMinimizer(CFirstOrderCostFunction *fun)
	{
		init();
		set_cost_function(fun);
	}

	/** Destructor */
	virtual ~CFirstOrderMinimizer()
	{}

	/** Do minimization and get the optimal value 
	 * 
	 * @return optimal value
	 */
	virtual float64_t minimize()=0;

	/** Does minimizer support batch update
	 * 
	 * @return whether minimizer supports batch update
	 */
	virtual bool supports_batch_update() const=0;

	/** Set cost function used in the minimizer
	 *
	 * @param fun the cost function
	 */
	virtual void set_cost_function(CFirstOrderCostFunction *fun)
	{
		m_fun=fun;
	}

	virtual CMinimizerContext* save_to_context()=0;

	virtual void load_from_context(CMinimizerContext* context)=0;
protected:
	/* Cost function */
	CFirstOrderCostFunction *m_fun;

private:
	/*  Init */
	void init()
	{
		m_fun=NULL;
	}
};

}
#endif
