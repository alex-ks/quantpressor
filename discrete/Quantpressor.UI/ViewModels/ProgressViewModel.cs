using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace Quantpressor.UI.ViewModels
{
	class ProgressViewModel : Screen
	{
		private double _progress;
		public double Progress
		{
			get { return _progress; }

			set
			{
				if ( Math.Abs( _progress - value ) > double.Epsilon )
				{
					_progress = value;
					NotifyOfPropertyChange( ( ) => Progress );
				}
			}
		}

		private string _status;

		public string Status
		{
			get { return _status; }

			set
			{
				if ( _status != value )
				{
					_status = value;
					NotifyOfPropertyChange( ( ) => Status );
				}
			}
		}
	}
}
