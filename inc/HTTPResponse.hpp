
#pragma once 

#include <string>

class	N {
	private:
		std::string	_aaa;
		std::string _bbb;
		std::string _ccc;
		std::string _ddd;

	public:
		N();
		N(std::string aaa,
			std::string bbb,
			std::string ccc,
			std::string ddd);
		N(const N &other);
		N &operator=(const N &other);
		~N();

		const std::string	&getaaa() const;
		const std::string	&getbbb() const;
		const std::string	&getccc() const;
		const std::string	&getddd() const;

		void		setaaa(const std::string &input);
		void		setbbb(const std::string &input);
		void		setccc(const std::string &input);
		void		setddd(const std::string &input);
};


