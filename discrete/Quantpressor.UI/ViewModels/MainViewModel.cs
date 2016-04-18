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
		private IWindowManager _manager;

		private double _error = 1e-2;
		public string Error
		{
			get { return _error.ToString( ); }
			set { _error = double.Parse( value ); }
		}

		public string CompressionScheme { get; set; } = Properties.Resources.HuffmanCode;

		public MainViewModel( IWindowManager manager )
		{
			_manager = manager;
		}

		private async Task<IGrid> OpenGrid( )
		{
			OpenFileDialog openFileDialog = new OpenFileDialog
			{
				Filter = "Text files|*.csv",
				ValidateNames = true
			};

			if ( openFileDialog.ShowDialog( ) != true )
				return null;

			var reader = new CsvGridReader( 1024 * 1024, ';' );
			//return reader.Read( openFileDialog.FileName, false, false );
			var openTask = new Task<IGrid>( ( ) => reader.Read( openFileDialog.FileName, false, false ) );
			openTask.Start( );
			return await openTask;
		}

		private readonly object _lockGuard = new object( );

		private Func<CompressionResultViewModel> CompressAndShow( IGrid grid, ICompressor compressor, string outName, ProgressViewModel progressBar )
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

				var quantizer = new Quantizer( leftBorders[column] - _error, rightBorders[column] + _error );
				var quantization = quantizer.Quantize( _error, distr );

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

			return ( ) => new CompressionResultViewModel( result, leftBorders, rightBorders, quantizations, distributions );
		}

		public async void Compress( )
		{
			ICompressor compressor;
			string extension;

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
				MessageBox.Show( "Unknown compression scheme!", "Error", MessageBoxButton.OK, MessageBoxImage.Error );
				return;
			}

			var grid = await OpenGrid( );

			if ( grid == null )
				return;

			var dialog = new SaveFileDialog
			{
				DefaultExt = extension,
				Filter = $"Quantized and compressed grid|*.{extension}",
				AddExtension = true
			};

			if ( dialog.ShowDialog( ) != true )
				return;

			string outName = dialog.FileName;

			var progressBar = new ProgressViewModel( );

			dynamic settings = new ExpandoObject( );
			settings.Height = 130;
			settings.Width = 500;
			settings.SizeToContent = SizeToContent.Manual;

			_manager.ShowWindow( progressBar, null, settings );

			var compressTask = Task.Factory.StartNew( ( ) => CompressAndShow( grid, compressor, outName, progressBar ) );

			var createResultViewModel = await compressTask;

			dynamic resultSettings = new ExpandoObject( );
			resultSettings.Height = 450;
			resultSettings.MinHeight = 180;
			resultSettings.Width = 800;
			resultSettings.MinWidth = 320;
			resultSettings.Title = "Report";
			resultSettings.SizeToContent = SizeToContent.Manual;

			_manager.ShowWindow( createResultViewModel( ), null, resultSettings );
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
	}
}
