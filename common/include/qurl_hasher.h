namespace Common
{

struct QUrlHasher
{
	std::size_t operator()(std::shared_ptr<QUrl> const& key)
	{
		return hasher(key->toString().toStdString());
	}

	boost::hash<std::string> hasher;
};

}