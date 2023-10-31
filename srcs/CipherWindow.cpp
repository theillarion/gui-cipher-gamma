#include "CipherWindow.hpp"
#include "ui_CipherWindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>

#include <random>

#include "Cipher/CipherXor.hpp"

namespace internal
{
	using CipherXorUtf16 = xk7::CipherXor<key_type, string_view_type>;
}

CipherWindow::CipherWindow(QWidget *parent)
    : 	QMainWindow{ parent },
		ui{ new Ui::CipherWindow },
		ciphers_
		{
			std::make_unique<internal::CipherXorUtf16>(),
			std::make_unique<internal::CipherXorUtf16>(),
		},
		ciphers2_
		{
			 std::make_unique<internal::CipherXorUtf16>(internal::CipherXorUtf16::BlockClutchMode),
			 std::make_unique<internal::CipherXorUtf16>(internal::CipherXorUtf16::BlockClutchMode),
		},
		key_validators_
		{   std::make_unique<QRegularExpressionValidator>(QRegularExpression("[a-z0-9]+")),
		    std::make_unique<QRegularExpressionValidator>(QRegularExpression("[а-яё0-9]+"))
		},
		text_validators_
		{   std::make_unique<QRegularExpressionValidator>(QRegularExpression("[^а-яё]+")),
		    std::make_unique<QRegularExpressionValidator>(QRegularExpression("[^a-z]+"))
		},
		current_cipher_{ ciphers_[0] },
		count_languages_{ 2 },
		idx_curr_lang_{ 0 }
{
    ui->setupUi(this);

	ui->cb_set_language->addItems({"English", "Russian"});
	ui->cb_set_language->setCurrentIndex(0);

	ui->label->setVisible(false);
	ui->rbt_test1->setVisible(false);
	ui->rbt_test2->setVisible(false);
//	connect(ui->rbt_test1, &QRadioButton::released, this, &CipherWindow::changedTypeInputKey);
//	connect(ui->rbt_test2, &QRadioButton::released, this, &CipherWindow::changedTypeInputKey);

	connect(ui->cb_set_language, &QComboBox::currentIndexChanged, this, &CipherWindow::changedLanguage);
	connect(ui->rbt_default, &QRadioButton::released, this, &CipherWindow::changedTypeCipher);
	connect(ui->rbt_block_chaining, &QRadioButton::released, this, &CipherWindow::changedTypeCipher);

	connect(ui->bt_convert_text_input_to_binary, &QPushButton::released, this,
			[this]{ convertTextToBinaryImpl(ui->text_input, ui->text_input_binary); });
	connect(ui->bt_convert_text_input2_to_binary, &QPushButton::released, this,
			[this]{ convertTextToBinaryImpl(ui->text_input2, ui->text_input2_binary); });
	connect(ui->bt_convert_text_key_to_binary, &QPushButton::released, this,
			[this]{ convertTextToBinaryImpl(ui->text_key, ui->text_key_binary); });
	connect(ui->bt_convert_text_decrypted_to_binary, &QPushButton::released, this,
			[this]{ convertTextToBinaryImpl(ui->text_decrypted, ui->text_decrypted_binary); });
	connect(ui->bt_convert_text_decrypted2_to_binary, &QPushButton::released, this,
			[this]{ convertTextToBinaryImpl(ui->text_decrypted2, ui->text_decrypted2_binary); });

	connect(ui->bt_encrypt, &QPushButton::released, this,
			[this]
			{
				auto input_key = ui->text_key->toPlainText();
				auto input_text = ui->text_input->toPlainText();
				encryptTextImpl(input_key, input_text, text_encrypted_, ui->text_encrypted_binary);
			}
	);

	connect(ui->bt_encrypt2, &QPushButton::released, this,
			[this]
			{
				auto input_text = ui->text_input2->toPlainText();
				encryptTextImpl(text_key2_, input_text, text_encrypted2_, ui->text_encrypted2_binary, false);
			}
	);

	connect(ui->bt_decrypt, &QPushButton::released, this,
			[this]
			{
				auto input_key = ui->text_key->toPlainText();
				decryptTextImpl(input_key, text_encrypted_, ui->text_decrypted);
			}
	);

	connect(ui->bt_decrypt2, &QPushButton::released, this,
			[this]
			{
				decryptTextImpl(text_key2_, text_encrypted2_, ui->text_decrypted2, false);
			}
	);

	connect(ui->bt_generate_text_key, &QPushButton::released, this,
			[this]
			{
				text_key2_ = generateRandomKey(std::mt19937{std::random_device{ }() }, ui->text_input2->toPlainText().size());
				ui->text_key2_binary->setText(toBinaryString(text_key2_));
			}
	);

	connect(ui->text_input, &QTextEdit::textChanged, this, [this] { ui->text_input_binary->clear(); });
	connect(ui->text_input2, &QTextEdit::textChanged, this, [this] { ui->text_input2_binary->clear(); });
	connect(ui->text_key, &QTextEdit::textChanged, this, [this] { ui->text_key_binary->clear(); });
	connect(ui->text_decrypted, &QTextEdit::textChanged, this, [this] { ui->text_decrypted_binary->clear(); });
	connect(ui->text_decrypted2, &QTextEdit::textChanged, this, [this] { ui->text_decrypted2_binary->clear(); });

	connect(ui->bt_load_text_input, &QPushButton::released, this, [this]{ loadFileImpl(ui->text_input); });
	connect(ui->bt_load_text_input2, &QPushButton::released, this, [this]{ loadFileImpl(ui->text_input2); });

	connect(ui->bt_save_text_input, &QPushButton::released, this, [this]{ saveTextToFileImpl(ui->text_input); });
	connect(ui->bt_save_text_input2, &QPushButton::released, this, [this]{ saveTextToFileImpl(ui->text_input2); });

	connect(ui->bt_clear_text_input, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_input); });
	connect(ui->bt_clear_text_input2, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_input2); });
	connect(ui->bt_clear_text_input_binary, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_input_binary); });
	connect(ui->bt_clear_text_input2_binary, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_input2_binary); });
	connect(ui->bt_clear_text_key, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_key); });
	connect(ui->bt_clear_text_key_binary, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_key_binary); });
	connect(ui->bt_clear_text_key2_binary, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_key2_binary); text_key2_.clear(); });
	connect(ui->bt_clear_text_encrypted_binary, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_encrypted_binary); text_encrypted_.clear(); });
	connect(ui->bt_clear_text_encrypted2_binary, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_encrypted2_binary); text_encrypted2_.clear(); });
	connect(ui->bt_clear_text_decrypted, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_decrypted); });
	connect(ui->bt_clear_text_decrypted2, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_decrypted2); });
	connect(ui->bt_clear_text_decrypted_binary, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_decrypted_binary); });
	connect(ui->bt_clear_text_decrypted2_binary, &QPushButton::released, this, [this]{ clearQTextEdit(ui->text_decrypted2_binary); });

	Q_ASSERT(
		count_languages_ == ciphers_.size() &&
		count_languages_ == key_validators_.size() &&
		count_languages_ == text_validators_.size() &&
		count_languages_ == ui->cb_set_language->count()
	);
}

CipherWindow::~CipherWindow()
{
    delete ui;
}

void CipherWindow::changedLanguage()
{
	auto idx_curr_language = ui->cb_set_language->currentIndex();
	if (idx_curr_language < 0 || idx_curr_language >= count_languages_)
	{
		QMessageBox::warning(this, "Выбор языка", "Ошибка при выборе языка. Повторите попытку!");
		qDebug() << QString("[warning]: index of the current language is %1").arg(idx_curr_language);
		return ;
	}
	if (idx_curr_language == idx_curr_lang_)
		return;

	idx_curr_lang_ = idx_curr_language;
	updateCurrentCipher();
}

void CipherWindow::changedTypeCipher()
{
	auto idx_curr_type_cipher = (ui->rbt_default->isChecked()) ? 0 : 1;
	if (idx_curr_type_cipher == idx_curr_type_cipher_)
		return;

	idx_curr_type_cipher_ = idx_curr_type_cipher;
	updateCurrentCipher();
}

//void CipherWindow::changedTypeInputKey()
//{
//	auto idx_curr_type_input_key = (ui->rbt_test1->isChecked()) ? 0 : 1;
//	if (idx_curr_type_input_key == 1)
//	{
//		//auto idx = ui->gridLayout_3->indexOf(ui->gridLayout_3.get);
//		ui->text_key->setHidden(true);
//
//		ui->gridLayout_3->removeWidget(ui->text_key);
//		ui->gridLayout_3->removeWidget(ui->text_key_binary);
//		ui->gridLayout_3->replaceWidget(ui->text_key_binary, ui->text_key);
//		ui->gridLayout_3->addWidget(ui->text_key_binary, 11, 1, 1, 2);
//		ui->text_key->setText("Test");
//		QMessageBox::information(this, ui->text_key->toPlainText(), "GG");
//	}
//	else
//	{
//		ui->text_key->setHidden(false);
//	}
//}

void CipherWindow::updateCurrentCipher()
{
	current_cipher_ = (idx_curr_type_cipher_ == 0) ? ciphers_[idx_curr_lang_] : ciphers2_[idx_curr_lang_];
	text_encrypted_.clear();
	text_encrypted2_.clear();
	ui->text_encrypted_binary->clear();
	ui->text_encrypted2_binary->clear();
	ui->text_decrypted->clear();
	ui->text_decrypted2->clear();
	ui->text_decrypted_binary->clear();
	ui->text_decrypted2_binary->clear();
}

void CipherWindow::convertTextToBinaryImpl(QTextEdit *src, QTextEdit *dst)
{
	if (!src || !src->document() || src->document()->isEmpty())
	{
		QMessageBox::information(this, "Перевод текста в бинарный вид", "Текстовое поле пустое");
		return;
	}
	dst->setText(toBinaryString(src->toPlainText()));
}

void CipherWindow::clearQTextEdit(QTextEdit *src)
{
	if (!src || !src->document() || src->document()->isEmpty())
	{
		QMessageBox::information(this, "Очистка поля", "Текстовое поле пустое");
		return;
	}
	src->clear();
}

//auto CipherWindow::encryptImpl(string_type& key, string_type& src_text) -> string_type
//{
//	auto idx_lang = static_cast<qsizetype>(idx_curr_lang_);
//	if (int pos = 0; key_validators_[idx_lang]->validate(key, pos) != QValidator::Acceptable)
//	{
//		QMessageBox::warning(this, "Ошибка шифрования/расшифровки", "Ключ не введен или является некорректным. Повторите попытку");
//		qDebug() << QString("[warning] invalid key on address 0x%1").arg(reinterpret_cast<qintptr>(key.data()));
//		return nullptr;
//	}
//	current_cipher_->setKey(key);
//
//	if (int pos = 0; text_validators_[idx_lang]->validate(src_text, pos) != QValidator::Acceptable)
//	{
//		QMessageBox::warning(this, "Ошибка валидации", QString("Текст не введен или найден(ы) запрещенный(ые) символ(ы)"));
//		qDebug() << QString("[warning] invalid input text on address 0x%1").arg(reinterpret_cast<qintptr>(src_text.data()));
//		return nullptr;
//	}
//	return current_cipher_->encrypt(src_text);
//}

//auto CipherWindow::decryptImpl(string_type& key, string_type& src_text) -> string_type
//{
//	auto idx_lang = static_cast<qsizetype>(idx_curr_lang_);
//	if (int pos = 0; key_validators_[idx_lang]->validate(key, pos) != QValidator::Acceptable)
//	{
//		QMessageBox::warning(this, "Ошибка шифрования/расшифровки", "Ключ не введен или является некорректным. Повторите попытку");
//		qDebug() << QString("[warning] invalid key on address 0x%1").arg(reinterpret_cast<qintptr>(key.data()));
//		return nullptr;
//	}
//	ciphers_[idx_lang]->setKey(key);
//	return ciphers_[idx_lang]->decrypt(src_text);
//}

//void CipherWindow::encryptToBinaryCodeImpl(QTextEdit *key, QTextEdit *src_text, QTextEdit *dst_binary_text)
//{
//	auto input_key = key->toPlainText();
//	auto input_text = src_text->toPlainText();
//	text_encrypted_ = encryptImpl(input_key, input_text);
//	if (text_encrypted_ != nullptr)
//		dst_binary_text->setText(toBinaryString(text_encrypted_));
//}

void CipherWindow::encryptTextImpl(string_type& key, string_type& src_text, string_type& dst_text, QTextEdit *dst_binary_text, bool check_key, bool check_text)
{
	if (!dst_binary_text)
	{
		QMessageBox::warning(this, "Ошибка шифрования/расшифровки", "Произошла неизвестная ошибка. Повторите попытку");
		qDebug() << QString("[warning] invalid address (destination binary text = 0x%1)")
			.arg(reinterpret_cast<qintptr>(dst_binary_text));
		return;
	}

	auto idx_lang = static_cast<qsizetype>(idx_curr_lang_);
	if (int pos = 0; key == nullptr || (check_key && key_validators_[idx_lang]->validate(key, pos) != QValidator::Acceptable))
	{
		QMessageBox::warning(this, "Ошибка шифрования/расшифровки", "Ключ не введен или является некорректным. Повторите попытку");
		qDebug() << QString("[warning] invalid key on address 0x%1").arg(reinterpret_cast<qintptr>(key.data()));
		return;
	}
	current_cipher_->setKey(key);

	if (int pos = 0; src_text == nullptr || (check_text && text_validators_[idx_lang]->validate(src_text, pos) != QValidator::Acceptable))
	{
		QMessageBox::warning(this, "Ошибка валидации", QString("Текст не введен или найден(ы) запрещенный(ые) символ(ы)"));
		qDebug() << QString("[warning] invalid input text on address 0x%1").arg(reinterpret_cast<qintptr>(src_text.data()));
		return;
	}
	dst_text = current_cipher_->encrypt(src_text);
	dst_binary_text->setText(toBinaryString(dst_text));
}

void CipherWindow::decryptTextImpl(string_type &key, string_type &src_text, QTextEdit *dst_text, bool check_key)
{
	if (!dst_text)
	{
		QMessageBox::warning(this, "Ошибка шифрования/расшифровки", "Произошла неизвестная ошибка. Повторите попытку");
		qDebug() << QString("[warning] invalid address (destination text = 0x%1")
			.arg(reinterpret_cast<qintptr>(dst_text));
		return;
	}

	auto idx_lang = static_cast<qsizetype>(idx_curr_lang_);
	if (int pos = 0; key == nullptr || (check_key && key_validators_[idx_lang]->validate(key, pos) != QValidator::Acceptable))
	{
		QMessageBox::warning(this, "Ошибка шифрования/расшифровки", "Ключ не введен или является некорректным. Повторите попытку");
		qDebug() << QString("[warning] invalid key on address 0x%1").arg(reinterpret_cast<qintptr>(key.data()));
		return;
	}

	if (src_text == nullptr || src_text.isEmpty())
	{
		QMessageBox::warning(this, "Ошибка валидации", QString("Текст не введен или найден(ы) запрещенный(ые) символ(ы)"));
		return;
	}

	current_cipher_->setKey(key);
	dst_text->setText(current_cipher_->decrypt(src_text));
}

void CipherWindow::loadFileImpl(QTextEdit *dst)
{
	auto filename = QFileDialog::getOpenFileName(this, "Выбор файла",
												 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Text files (*.txt)"));

	if (filename == nullptr)
	{
		QMessageBox::information(this, "Выбор файла", "Выбор файла был отменен (test)");
		return;
	}

	QFile in_file{ filename };

	if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::warning(this, "Открытие файла", "Произошла ошибка при открытие файла. Повторите попытку!");
		qDebug() << QString("[warning] error (code %1) when opening a file on the path \"%2\": %3")
			.arg(in_file.error()).arg(in_file.fileName(), in_file.errorString());
		return;
	}

	QTextStream in_stream{ &in_file };
	dst->setText(in_stream.readAll());

	if (in_stream.status() != QTextStream::Ok)
	{
		QMessageBox::warning(this, "Чтение файла", "Произошла ошибка при чтении файла. Повторите попытку!");
		qDebug() << QString("[warning] error (code %1) when reading a file at position %2")
			.arg(in_stream.status()).arg(in_stream.pos());
		return;
	}
	in_file.close();
	QMessageBox::information(this, "Чтение файла", "Файл успешно прочитан");
}

void CipherWindow::saveTextToFileImpl(QTextEdit *src)
{
	if (!src || !src->document() || src->document()->isEmpty())
	{
		QMessageBox::information(this, "Сохранение текста", "Текстовое поле пустое");
		return;
	}

	QFileDialog dialog{};

	auto filename = QFileDialog::getSaveFileName(this, "Сохранить как",
												 QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation), tr("Text files (*.txt)"));

	if (filename == nullptr)
	{
		QMessageBox::information(this, "Сохранение текста", "Выбор файла был отменен");
		return;
	}

	QFile out_file{ filename };

	if (!out_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		QMessageBox::warning(this, "Ошибка при открытие файла", "Произошла ошибка при открытие файла. Повторите попытку");
		qDebug() << QString("[warning] error (code %1) when opening a file on the path \"%2\": %3")
			.arg(out_file.error()).arg(out_file.fileName(), out_file.errorString());
		return;
	}

	QTextStream out_stream{ &out_file };

	out_stream << src->toPlainText();
	if (out_stream.status() != QTextStream::Ok)
	{
		QMessageBox::warning(this, "Запись текста в файл", "При записи текста в файл произошла ошибка. Повторите попытку!");
		qDebug() << QString("[warning] error (code %1) when reading a file at position %2")
			.arg(out_stream.status()).arg(out_stream.pos());
		return;
	}
	out_file.close();
	QMessageBox::information(this, "Запись файла", QString{ "Файл успешно записан по пути:\n%1" }.arg(filename));
}

internal::string_type CipherWindow::toBinaryString(internal::string_view_type str)
{
	auto count_bytes = sizeof(internal::string_view_type::value_type);
	auto count_bits = count_bytes * 8;
	qsizetype length = (count_bits + (count_bytes * 2) + count_bytes) * str.size();

	internal::string_type result{};
	result.reserve(length);

	for (const auto symbol: str)
	{
		result += QString{ "0b%1 0b%2 " }.arg(QString::number((symbol.unicode() >> 8) & 0xFF, 2).rightJustified(8, '0'),
											  QString::number(symbol.unicode() & 0xFF, 2).rightJustified(8, '0'));
		//result += QString::number(symbol.unicode(), 2) + ' ';
	}
	return result.trimmed();
}


template<class Generator>
internal::string_type CipherWindow::generateRandomKey(Generator gen, CipherWindow::size_type length)
{
	std::uniform_int_distribution<unsigned short> distribution(0, static_cast<unsigned short>((1 << 16) - 1));
	string_type result;
	result.reserve(length);

	std::generate_n(std::back_inserter(result), length, [&gen, distribution] () mutable { return string_type::value_type(distribution(gen)); });

	return result;
}