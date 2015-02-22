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

#include <shogun/machine/gp/GaussianARDFITCKernel.h>

using namespace shogun;

CGaussianARDFITCKernel::CGaussianARDFITCKernel() : CGaussianARDKernel()
{
	init();
}
void CGaussianARDFITCKernel::init()
{
}

CGaussianARDFITCKernel::~CGaussianARDFITCKernel()
{
}

#ifdef HAVE_CXX11
#include <shogun/mathematics/linalg/linalg.h>
CGaussianARDFITCKernel::CGaussianARDFITCKernel(int32_t size, float64_t width)
		: CGaussianARDKernel(size,width)
{
	init();
}

CGaussianARDFITCKernel::CGaussianARDFITCKernel(CDotFeatures* l,
		CDotFeatures* r, int32_t size, float64_t width)
		: CGaussianARDKernel(l, r, size, width)
{
	init();
}

CGaussianARDFITCKernel* CGaussianARDFITCKernel::obtain_from_generic(CKernel* kernel)
{
	if (kernel->get_kernel_type()!=K_GAUSSIANARDFITC)
	{
		SG_SERROR("Provided kernel is not of type CGaussianARDFITCKernel!\n");
	}

	/* since an additional reference is returned */
	SG_REF(kernel);
	return (CGaussianARDFITCKernel*)kernel;
}

#include <typeinfo>
CSGObject *CGaussianARDFITCKernel::shallow_copy() const
{
	// TODO: remove this after all the classes get shallow_copy properly implemented
	// this assert is to avoid any subclass of CGaussianARDFITCKernel accidentally called
	// with the implement here
	ASSERT(typeid(*this) == typeid(CGaussianARDFITCKernel))
	CGaussianARDFITCKernel *ker = new CGaussianARDFITCKernel(cache_size, m_width);
	if (lhs)
	{
		ker->CGaussianARDKernel::init(lhs, rhs);
	}
	return ker;
}

SGMatrix<float64_t> CGaussianARDFITCKernel::get_parameter_gradient(
		const TParameter* param, index_t index)
{
	if (!strcmp(param->m_name, "latent_features"))
	{
		REQUIRE(lhs && rhs, "Features not set!\n")
		REQUIRE(index>=0 && index<num_lhs,"")
		int32_t idx_l=index;
		SGVector<float64_t> left_vec=((CDotFeatures *)lhs)->get_computed_dot_feature_vector(idx_l);
		SGMatrix<float64_t> res(left_vec.vlen, num_rhs);

		for (int32_t idx_r=0; idx_r<num_rhs; idx_r++)
		{
			SGVector<float64_t> right_vec=((CDotFeatures *)rhs)->get_computed_dot_feature_vector(idx_r);
			SGMatrix<float64_t> res_transpose(res.get_column_vector(idx_r),1,left_vec.vlen,false);
			linalg::add(left_vec, right_vec, right_vec, 1.0, -1.0);
			float64_t scalar_weight=1.0;
			//column vector
			SGMatrix<float64_t> right=compute_right_product(right_vec, scalar_weight);
			//row vector
			SGMatrix<float64_t> right_transpose(right.matrix,1,right.num_rows,false);
			if (m_ARD_type==KT_SCALAR)
			{
				scalar_weight*=m_weights[0];
				SGMatrix<float64_t> weights(1,right.num_rows);
				weights.set_const(scalar_weight);
				linalg::matrix_product(right_transpose, weights, res_transpose);
			}
			else
			{
				if(m_ARD_type==KT_DIAG)
				{
					SGMatrix<float64_t> weights(m_weights.matrix,1,m_weights.num_rows,false);
					linalg::elementwise_product(right_transpose, weights, res_transpose);
				}
				else if(m_ARD_type==KT_FULL)
				{
					linalg::matrix_product(right_transpose, m_weights, res_transpose);
				}
				else
				{
					SG_ERROR("Unsupported ARD kernel\n");
				}

			}

			for (index_t i=0; i<left_vec.vlen; i++)
				res(i,idx_r)*=kernel(idx_l,idx_r)*-2.0/m_width;
		}
		return res;
	}
	else
	{
		return CGaussianARDKernel::get_parameter_gradient(param, index);
	}
}
#endif /* HAVE_CXX11 */
