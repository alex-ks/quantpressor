#pragma once

#include <string>

namespace module_api
{
	class Exception
	{
	private:
		std::wstring message;

	public:
		Exception( );
		Exception( const std::wstring &message );

		virtual std::wstring get_message( ) const;
	};

	class IndexOutOfRangeException : public Exception
	{
	public:
		IndexOutOfRangeException( );
		IndexOutOfRangeException( const std::wstring &message );
	};

	class BadInputException : public Exception
	{
	public:
		BadInputException( );
		BadInputException( const std::wstring &message );
	};

	class ComputationException : public Exception
	{
	public:
		ComputationException( );
		ComputationException( const std::wstring &message );
	};

	class NotImplementedException : public Exception
	{
	public:
		NotImplementedException( );
		NotImplementedException( const std::wstring &message );
	};

	class UnexpectedException : public Exception
	{
	public:
		UnexpectedException( );
		UnexpectedException( const std::wstring &message );
	};
}