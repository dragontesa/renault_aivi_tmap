#ifndef NAVIBOOKMARKPROTOCOL_H
#define NAVIBOOKMARKPROTOCOL_H
namespace SKT {

class NavibookmarkProtocol {

public:
enum NavibookTransactionType {
	TransactionUnknown,
	TransactionSearch,
	TransactionMap,
	TransactionRecentAddress,
	TransactionRecentSearch,
	TransactionAddressbook,
	TransactionUserInput,
};

enum NavibookBookmarkType {
	BookmarkUnknown,
    BookmarkRecentDestination,
	BookmarkRecentSearch,
	BookmarkFavorite,
};

enum NavibookSearchType {
	SearchUnknown,
	AddressSearch,
	SisulNameSearch,
	TextSearch,
	OilSearch,
	BusinessSearch,
	MapSearch,
};

enum NavibookAddressType {
	AddressTypeUnknown,
	NewRoadAddressType,
	OldAddressType,
};

};
}

#endif // NAVIBOOKMARKPROTOCOL_H
