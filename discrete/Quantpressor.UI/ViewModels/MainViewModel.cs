using System;
using System.Collections.Generic;
using System.Dynamic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using Caliburn.Micro;
using Microsoft.Win32;
using Quantpressor.Compressors;
using Quantpressor.Distributions;
using Quantpressor.FileIO;

namespace Quantpressor.UI.ViewModels
{
	class MainViewModel : PropertyChangedBase
	{
		private readonly IWindowManager _manager;

		public string CompressionScheme { get; set; } = Properties.Resources.HuffmanCode;

		public MainViewModel( IWindowManager manager )
		{
			_manager = manager;
		}

		private Task<IGrid> OpenGrid( )
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "Text files|*.csv",
				ValidateNames = true
			};

			var fileName = openFileDialog.ShowDialog( ) == true ? openFileDialog.FileName : null;
			var reader = new CsvGridReader( 1024 * 1024, ';' );

			return Task<IGrid>.Factory.StartNew( ( ) => fileName != null ? reader.Read( fileName, false, false ) : null );
		}

		private readonly object _lockGuard = new object( );

		private CompressionStats Compress( IGrid grid,
		                                   ICompressor compressor,
										   double[] errors,
										   string outName,
		                                   ProgressViewModel progressBar )
		{
			double[] leftBorders = new double[grid.ColumnCount];
			double[] rightBorders = new double[grid.ColumnCount];

			var qs = new IQuantization[grid.ColumnCount];
			var distrs = new IDistribution[grid.ColumnCount];

			progressBar.Status = "Quantizing columns...";

			Parallel.For( 0, grid.ColumnCount, column =>
			{
				var distr = new EmpiricalDistribution( grid, column );

				leftBorders[column] = double.MaxValue;
				rightBorders[column] = double.MinValue;

				for ( int row = 0; row < grid.RowCount; ++row )
				{
					double value = grid.GetValue( row, column );
					leftBorders[column] = leftBorders[column] < value ? leftBorders[column] : value;
					rightBorders[column] = rightBorders[column] > value ? rightBorders[column] : value;
				}

				var quantizer = new Quantizer( leftBorders[column], rightBorders[column] );
				var quantization = quantizer.Quantize( errors[column], distr );

				lock ( _lockGuard )
				{
					progressBar.Progress += 1.0 / ( grid.ColumnCount + 1 );
					distrs[column] = distr;
					qs[column] = quantization;
				}
			} );

			var quantizations = new List<IQuantization>( qs );
			var distributions = new List<IDistribution>( distrs );

			progressBar.Status = "Writing archive...";
			progressBar.Progress = ( double )grid.ColumnCount / ( grid.ColumnCount + 1 );

			ICompressionResult result;

			using ( var stream = new FileOutputStream( outName ) )
			{
				result = compressor.Compress( grid, quantizations, stream );
			}

			progressBar.Progress = 1.0;
			progressBar.TryClose( );

			return new CompressionStats
			{
				CompressionResult = result,
				Distributions = distributions,
				LeftBorders = leftBorders,
				RightBorders = rightBorders,
				Quantizations = quantizations
			};
		}

		public async void Compress( )
		{
			ICompressor compressor;
			string extension;

			var grid = await OpenGrid( );

			if ( grid == null )
				return;

			ChooseCompressor( out compressor, out extension );
			if ( compressor == null || extension == null )
			{ MessageBox.Show( "Unknown compression scheme!", "Error", MessageBoxButton.OK, MessageBoxImage.Error ); }

			var columnNames = new List<string>( );
			for ( var i = 0; i < grid.ColumnCount; ++i )
			{ columnNames.Add( $"Column #{i}" ); }

			dynamic paramsSettings = new ExpandoObject( );
			paramsSettings.Height = 200;
			paramsSettings.Width = 420;
			paramsSettings.SizeToContent = SizeToContent.Manual;
			paramsSettings.Title = "Compression parameters";

			var compParams = new CompressionParamsViewModel( columnNames );

			bool? result = _manager.ShowDialog( compParams,
												null,
												paramsSettings );

			if ( result == null || !result.Value )
			{ return; }

			var dialog = new SaveFileDialog
			{
				DefaultExt = extension,
				Filter = $"Quantized and compressed grid|*.{extension}",
				AddExtension = true
			};

			if ( dialog.ShowDialog( ) != true )
				return;

			var errors = ( from desc in compParams.Errors
			               select desc.Error ).ToArray( );

			string outName = dialog.FileName;

			var progressBar = new ProgressViewModel( );

			dynamic settings = new ExpandoObject( );
			settings.Height = 130;
			settings.Width = 500;
			settings.SizeToContent = SizeToContent.Manual;

			_manager.ShowWindow( progressBar, null, settings );

			var stats =
				await Task<CompressionStats>.Factory.StartNew( ( ) => Compress( grid, compressor, errors, outName, progressBar ) );

			dynamic resultSettings = new ExpandoObject( );
			resultSettings.Height = 450;
			resultSettings.MinHeight = 180;
			resultSettings.Width = 800;
			resultSettings.MinWidth = 320;
			resultSettings.Title = "Report";
			resultSettings.SizeToContent = SizeToContent.Manual;

			_manager.ShowWindow( new CompressionResultViewModel( stats ), null, resultSettings );
		}

		public const int DefaultWidth = 1024 * 10;

		public async void Decompress( )
		{
			var openFileDialog = new OpenFileDialog
			{
				Filter = $"Quantized huffman-encoded file|*.{Properties.Resources.HuffmanCodeExt}|" +
						 $"Quantized LZ77-encoded file|*.{Properties.Resources.Lz77Ext}" +
						 $"Quantized arithmetic-encoded file|*.{Properties.Resources.ArithmeticCodeExt}",
				ValidateNames = true
			};

			if ( openFileDialog.ShowDialog( ) != true )
				return;

			try
			{
				using ( var input = new FileInputStream( openFileDialog.FileName ) )
				{
					var dialog = new SaveFileDialog
					{
						DefaultExt = "csv",
						Filter = "Text file|*.csv",
						AddExtension = true
					};

					if ( dialog.ShowDialog( ) != true )
						return;

					var ext = Path.GetExtension( openFileDialog.FileName );

					ICompressor compressor;

					if ( ext == Properties.Resources.ArithmeticCodeExt )
					{ compressor = new ArithmeticCodingCompressor( ); }
					else if ( ext == Properties.Resources.Lz77Ext )
					{ compressor = new LZ77HuffmanCompressor( DefaultWidth ); }
					else
					{ compressor = new HuffmanCompressor( ); }

					//var grid = compressor.Decompress( input );

					throw new NotImplementedException( "Csv writing is not implemented!" );
				}
			}
			catch ( Exception e )
			{
				MessageBox.Show( e.Message, "Error", MessageBoxButton.OK, MessageBoxImage.Error );
			}
		}

		private void ChooseCompressor( out ICompressor compressor, out string extension )
		{
			if ( CompressionScheme == Properties.Resources.HuffmanCode )
			{
				compressor = new HuffmanCompressor( );
				extension = Properties.Resources.HuffmanCodeExt;
			}
			else if ( CompressionScheme == Properties.Resources.Lz77 )
			{
				compressor = new LZ77HuffmanCompressor( DefaultWidth );
				extension = Properties.Resources.Lz77Ext;
			}
			else if ( CompressionScheme == Properties.Resources.ArithmeticCode )
			{
				compressor = new ArithmeticCodingCompressor( );
				extension = Properties.Resources.ArithmeticCodeExt;
			}
			else
			{
				extension = null;
				compressor = null;
			}
		}
	}
}
