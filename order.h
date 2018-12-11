#ifndef ORDER_HPP
#define ORDER_HPP

#include <iostream>
#include <vector>

typedef std::vector< int > Order;

template< typename TValue >
bool
smaller( TValue const & p_v1, TValue const & p_v2 ) {

	return p_v1 < p_v2;
}

template< typename TContainer, typename TValue >
void
swap( std::size_t p_i, std::size_t p_j, TContainer & p_values ) {

	TValue
	value           = p_values[ p_i ];
	p_values[ p_i ] = p_values[ p_j ];
	p_values[ p_j ] = value;
}

Order
naturalOrder( std::size_t const & p_size );

template< typename TContainer, typename TValue >
void
order( TContainer const & p_values, int p_lo, int p_hi, Order & p_indices, bool ( * p_smaller ) ( TValue const & p_v1, TValue const & p_v2 ) ) {

	int
	i = p_lo,
	j = p_hi;

	TValue const
	& x = p_values[ p_indices[ ( p_lo + p_hi ) / 2 ] ];

	//  Aufteilung
	while( i <= j ) {

		while( p_smaller( p_values[ p_indices[ i ] ], x ) ) {

			++ i;
		}

		while( p_smaller( x, p_values[ p_indices[ j ] ] ) ) {

			-- j;
		}

		if( i <= j ) {

			swap< Order, std::size_t >( i, j, p_indices );

			++ i;

			-- j;
		}
	}

	// Rekursion
	if( p_lo < j ) {

		order( p_values, p_lo, j, p_indices, p_smaller );
	}

	if( i < p_hi ) {

		order( p_values, i, p_hi, p_indices, p_smaller );
	}
}

template< typename TContainer, typename TValue >
Order
order( TContainer const & p_values, bool ( * p_smaller ) ( TValue const & p_v1, TValue const & p_v2 ) ) {

	Order
	idx = naturalOrder( p_values.size( ) );

	order( p_values, 0, p_values.size( ) - 1, idx, p_smaller );

	return	idx;
}

template< typename TContainer, typename TValue >
Order
rank( TContainer const & p_values, bool ( * p_smaller ) ( TValue const & p_v1, TValue const & p_v2 ) ) {

	Order
	idx = order( p_values, p_smaller ),
	rnk = naturalOrder( idx.size( ) );

	for( std::size_t  i = 0; i < rnk.size( ); ++ i ) {

		rnk[ idx[ i ] ] = i;
	}

	return rnk;
}

template< typename TValue >
Order
rank( TValue const * p_values, std::size_t const & p_size, bool ( * p_smaller ) ( TValue const & p_v1, TValue const & p_v2 ) ) {

	Order
	idx = order( p_values, p_size, p_smaller ),
	rnk = naturalOrder( idx.size( ) );

	for( int i = 0; i < rnk.size( ); ++ i ) {

		rnk[ idx[ i ] ] = i;
	}

	return rnk;
}

#endif // QSORT_HPP
