import hipop
import importlib.metadata


def test_version_consistency() -> None:

    package_version = hipop.__version__
    distribution_version = importlib.metadata.version("scisoftdev-tmp-hipop") # Name of the project in `pyproject.toml` TODO change it
    assert package_version == distribution_version, "Package version and distribution version must be identical"
