#ifndef ORDER_H
#define ORDER_H

#include "order.h"
#include <iostream>
#include <vector>

Order
naturalOrder( std::size_t const & p_size ) {

	Order
	idx( p_size );

	for( int i = 0; i < p_size; ++ i ) {

		idx[ i ] = i;
	}

	return idx;
}

#endif // QSORT_HPP
