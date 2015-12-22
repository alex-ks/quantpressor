#include "RoughResult.h"

#include <utility>

using namespace igor;
using std::wstring;

using module_api::pIGrid;
using module_api::uint;
using module_api::Paragraphs;
using module_api::InputException;

RoughResult::RoughResult( ) : grids( 0 )
{
	plain_text.push_back( L"Exorcizamus te, omnis immundus spiritus, omnis satanica potestas, omnis incursio infernalis adversarii, omnis legio, omnis congregatio et secta diabolica, in nomine et virtute Domini Nostri Jesu." );
	plain_text.push_back( L"Christi, eradicare et effugare a Dei Ecclesia, ab animabus ad imaginem Dei conditis ac pretioso divini Agni sanguine redemptis." );
	plain_text.push_back( L"Non ultra audeas, serpens callidissime, decipere humanum genus, Dei Ecclesiam persequi, ac Dei electos excutere et cribrare sicut triticum" );
	plain_text.push_back( L"Imperat tibi Deus altissimus..." );
	plain_text.push_back( L"Cui in magna tua superbia te similem haberi adhuc praesumis; qui omnes homines vult salvos fieri et ad agnitionem veritaris venire. Imperat tibi Deus Pater..." );
	plain_text.push_back( L"Imperat tibi Deus Filius..." );
	plain_text.push_back( L"Imperat tibi Deus Spiritus Sanctus..." );
	plain_text.push_back( L"Imperat tibi majestas Christi, aeternum Dei Verbum, caro factum..." );
	plain_text.push_back( L"Qui pro salute generis nostri tua invidia perditi, humiliavit semetipsum facfus hobediens usque ad mortem; qui Ecclesiam suam aedificavit supra firmam petram, et portas inferi adversus eam nunquam esse praevalituras edixit, cum ea ipse permansurus omnibus diebus usque ad consummationem saeculi. Imperat tibi sacramentum Crucis..." );
	plain_text.push_back( L"Omniumque christianae fidei Mysteriorum virtus." );
	plain_text.push_back( L"Imperat tibi excelsa Dei Genitrix Virgo Maria..." );
	plain_text.push_back( L"Quae superbissimum caput tuum a primo instanti immaculatae suae conceptionis in sua humilitate contrivit. Imperat tibi fides sanctorum Apostolorum Petri et Pauli, et ceterorum Apostolorum." );
	plain_text.push_back( L"Imperat tibi Martyrum sanguis, ac pia Sanctorum et Sanctarum omnium intercessio." );
}

igor::RoughResult::RoughResult( module_api::Paragraphs &&text ) : grids( 0 ), plain_text( std::move( text ) )
{ 
	
}

igor::RoughResult::RoughResult( const module_api::Paragraphs &text ) : grids( 0 ), plain_text( text )
{

}

RoughResult::~RoughResult( )
{

}

pIGrid RoughResult::get_grid( uint i ) const
{
	if ( i >= grids.size( ) )
		throw InputException::INDEX_OUT_OF_RANGE;

	return grids[i];
}

uint RoughResult::get_grids_count( ) const
{
	return grids.size( );
}

Paragraphs RoughResult::get_plain_text( ) const
{
	return plain_text;
}

void RoughResult::add_grid( const pIGrid &grid )
{
	grids.push_back( grid );
}

void igor::RoughResult::set_plain_text( module_api::Paragraphs &&text )
{
	plain_text = std::move( text );
}

void igor::RoughResult::set_plain_text( const module_api::Paragraphs &text )
{
	plain_text = text;
}
