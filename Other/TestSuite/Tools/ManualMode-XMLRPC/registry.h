
class Registry {
public:
	Registry(void);

	void read_reg(void);
	void write_reg(void);

	// Window location and size
	int			manual_x, manual_y;

	// Last used macro
	CString		macro;

	// Popup menu location
	bool		unobstructivePopup;

};

