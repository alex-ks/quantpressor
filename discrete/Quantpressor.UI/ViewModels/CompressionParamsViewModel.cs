using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace Quantpressor.UI.ViewModels
{
	public class ColumnDesc : PropertyChangedBase
	{
		public string Name { get; set; }

		private double _error;

		public double Error
		{
			get
			{
				return _error;
			}
			set
			{
				if ( _error != value )
				{
					_error = value;
					NotifyOfPropertyChange( nameof( Error ) );
				}
			}
		}
	}

	public class CompressionParamsViewModel : Screen
	{
		private ColumnDesc[] _errors;

		public ColumnDesc[] Errors
		{
			get
			{
				if ( _commonErrorEnabled )
				{
					foreach ( var desc in _errors )
					{ desc.Error = _commonError; }
				}
				return _errors;
			}
			set { _errors = value; }
		}

		private double _commonError = 1e-2;

		public string CommonError
		{
			get { return _commonError.ToString( ); }
			set
			{
				double temp;
				if ( double.TryParse( value, out temp ) )
				{ _commonError = temp; }
			}
		}

		private bool _commonErrorEnabled = true;
		public bool EnableCommonError
		{
			get { return _commonErrorEnabled; }
			set
			{
				if ( value != _commonErrorEnabled )
				{
					_commonErrorEnabled = value;
					NotifyOfPropertyChange( nameof( EnableCommonError ) );
					NotifyOfPropertyChange( nameof( CommonErrorDisabled ) );

					if ( !value )
					{
						foreach ( var desc in _errors )
						{ desc.Error = _commonError; }
					}
				}
			}
		}

		public bool CommonErrorDisabled => !_commonErrorEnabled;

		public void Accept( )
		{
			TryClose( true );
		}

		public CompressionParamsViewModel( List<string> columnNames )
		{
			_errors = new ColumnDesc[columnNames.Count];

			for ( int i = 0; i < _errors.Length; ++i )
			{
				_errors[i] = new ColumnDesc
				{
					Error = _commonError,
					Name = columnNames[i]
				};
			}
		}
	}
}
