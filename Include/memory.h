#pragma once

#include <memory>
#include <functional>

namespace module_api
{
	template <typename Type> using Deleter = std::function<void( Type* )>;

	///<summary>
	///"Умный" указатель на базе std::shared_ptr. Производит удаление объекта точно на той куче, где он был создан.
	///</summary>
	template<typename Type> class HeapPtr : public std::shared_ptr<Type>
	{
	public:
		HeapPtr( ) : std::shared_ptr<Type>( )
		{

		}

		HeapPtr( Type *object ) : std::shared_ptr<Type>( object, []( Type *ptr ) { delete ptr; } )
		{

		}

		//nullable

		HeapPtr( nullptr_t null ) : std::shared_ptr<Type>( null )
		{

		}

		HeapPtr &operator=( nullptr_t null ) noexcept
		{
			HeapPtr( ).swap( *this );
			return *this;
		}

		//copy semantics

		HeapPtr( const HeapPtr &origin ) noexcept : std::shared_ptr<Type>( origin )
		{

		}

		template<typename Type2> HeapPtr( const HeapPtr<Type2> &origin ) noexcept
		{
			std::_Ptr_base<Type>::_Reset( origin );
		}

		HeapPtr &operator=( const HeapPtr& origin ) noexcept
		{
			HeapPtr( origin ).swap( *this );
			return *this;
		}

		template<typename Type2> HeapPtr &operator=( const HeapPtr<Type2> &origin ) noexcept
		{
			HeapPtr( origin ).swap( *this );
			return *this;
		}

		//move semantics

		HeapPtr( HeapPtr &&source ) noexcept : std::shared_ptr<Type>( std::move( source ) )
		{

		}

		template<typename Type2> HeapPtr( HeapPtr<Type2> &&source ) noexcept : std::shared_ptr<Type>( std::move( source ) )
		{

		}

		HeapPtr &operator=( HeapPtr &&source ) noexcept
		{
			HeapPtr( std::move( source ) ).swap( *this );
			return *this;
		}

		template<typename Type2> HeapPtr &operator=( HeapPtr<Type2> &&source ) noexcept
		{
			HeapPtr( std::move( source ) ).swap( *this );
			return *this;
		}
	};

	///<summary>
	///"Умный" указатель на базе std::shared_ptr. Производит удаление объекта точно на той куче, где он был создан. Специализация для массивов.
	///</summary>
	template<typename T> class HeapPtr<T[]> : public std::shared_ptr<T>
	{
	public:
		HeapPtr( T *object ) : std::shared_ptr<T>( object, []( T *ptr ) { delete[] ptr; } )
		{

		}

		HeapPtr( size_t size ) : HeapPtr( new T[size] )
		{

		}

		HeapPtr( ) : std::shared_ptr<T>( )
		{

		}

		T &operator[]( size_t index )
		{
			return get( )[index];
		}
	};

	///<summary>
	///<para>Возвращает указатель на объект, сконструированный с заданными аргументами.</para>
	///</summary>
	///<param name="args">Аргументы, от которых будет вызван конструктор объекта.</param>
	///<returns>Указатель на созданный объект.</returns>
	template<typename Type, typename... ArgTypes> inline HeapPtr<Type> make_heap_aware( ArgTypes &&... args )
	{
		Type *ptr = new Type( std::forward<ArgTypes>( args )... );
		return std::move( HeapPtr<Type>( ptr ) );
	}

	template<typename Type1, typename Type2> bool operator==( const HeapPtr<Type1> &left, const HeapPtr<Type2> &right ) noexcept
	{
		return( left.get( ) == right.get( ) );
	}

	template<typename Type1, typename Type2> bool operator!=( const HeapPtr<Type1> &left, const HeapPtr<Type2> &right ) noexcept
	{
		return( left.get( ) != right.get( ) );
	}

	template<typename Type1> bool operator==( const HeapPtr<Type1> &left, nullptr_t ) noexcept
	{
		return( left.get( ) == nullptr );
	}

	template<typename Type1> bool operator==( nullptr_t, const HeapPtr<Type1> &right ) noexcept
	{
		return( nullptr == right.get( ) );
	}

	template<typename Type1> bool operator!=( const HeapPtr<Type1> &left, nullptr_t ) noexcept
	{
		return( left.get( ) != nullptr );
	}

	template<typename Type1> bool operator!=( nullptr_t, const HeapPtr<Type1> &right ) noexcept
	{
		return( nullptr != right.get( ) );
	}
}
