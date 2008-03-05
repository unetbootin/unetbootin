#include <QtCore>

int main(int argc, char *argv[])
{
	QFile infile(argv[1]);
	infile.open(QIODevice::ReadWrite);
	QByteArray b64Data = qCompress(infile.readAll()).toBase64();
	infile.close();
	QFile outFile(QString("%1.cpp").arg(argv[2]));
	outFile.open(QIODevice::ReadWrite);
	QTextStream output(&outFile);
	output << "QByteArray " << argv[2] << " = qUncompress(QByteArray::fromBase64(\"" << b64Data << "\"));" << endl;
	outFile.close();
}
