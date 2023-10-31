#pragma once

#include <QMainWindow>
#include <QString>
#include <QStringView>
#include <QVector>
#include <QValidator>
#include <QTextEdit>

#include <memory>

#include "Alphabet/IAlphabet.hpp"
#include "Cipher/CipherXor.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class CipherWindow; }
QT_END_NAMESPACE

namespace internal
{
	using string_type = QString;
	using string_view_type = QStringView;
	using char_type = typename string_type::value_type;
	using key_type = string_type;
	using size_type = std::size_t;

	using IAlphabetUtf16 = xk7::IAlphabet<char_type>;
	using ICipherUtf16 = xk7::ICipher<internal::key_type, internal::string_type, internal::string_view_type>;
}

class CipherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CipherWindow(QWidget *parent = nullptr);
    ~CipherWindow() override;

private slots:
	void changedLanguage();
	void changedTypeCipher();
	//void changedTypeInputKey();

private:
	using size_type = internal::size_type;
	using string_type = internal::string_type;

	void updateCurrentCipher();
	void convertTextToBinaryImpl(QTextEdit* src, QTextEdit *dst);
	void clearQTextEdit(QTextEdit *);

	void encryptTextImpl(string_type& key, string_type& src_text, string_type& dst_text, QTextEdit* dst_binary_text,
						 bool check_key = true, bool check_text = true);
	void decryptTextImpl(string_type& key, string_type& src_text, QTextEdit* dst_text, bool check_key = true);

	void loadFileImpl(QTextEdit *);
	void saveTextToFileImpl(QTextEdit *);

	static internal::string_type toBinaryString(internal::string_view_type);
	template <class Generator>
	static internal::string_type generateRandomKey(Generator random, size_type length);

private:
    Ui::CipherWindow *ui;
	QVector<std::shared_ptr<internal::ICipherUtf16>> ciphers_;
	QVector<std::shared_ptr<internal::ICipherUtf16>> ciphers2_;
	QVector<std::shared_ptr<QValidator>> key_validators_;
	QVector<std::shared_ptr<QValidator>> text_validators_;
	std::shared_ptr<internal::ICipherUtf16> current_cipher_;
	internal::size_type count_languages_ = 0;
	internal::size_type   idx_curr_lang_ = 0;
	internal::size_type   idx_curr_type_cipher_ = 0;
	internal::string_type text_encrypted_;
	internal::string_type text_key2_;
	internal::string_type text_encrypted2_;
};