#ifndef ORDER_H
#define ORDER_H

#include "order.h"
#include <iostream>
#include <vector>

std::vector< std::size_t >
naturalOrder( int p_size ) {

	std::vector< std::size_t >
	idx( p_size );

	for( int i = 0; i < p_size; ++ i ) {

		idx[ i ] = i;
	}

	return idx;
}

#endif // QSORT_HPP
