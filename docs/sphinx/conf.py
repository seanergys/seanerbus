# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

from this import s


project = "SeanerBUS"
copyright = "2026, SEANERGYS Partners"
author = "SEANERGYS Partners"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "breathe",
    "sphinxcontrib_rust",
    "sphinx.ext.autosummary",
    "myst_parser",
    "sphinx.ext.autodoc",
]

breathe_projects = {
    "cpp_seanerbus": "../../bindings/cpp/xml",
}

source_suffix = {
    ".rst": "restructuredtext",
    ".md": "markdown",
    ".txt": "markdown",  # Optional
}
# See docs/compatibility for details on these extensions.
myst_enable_extensions = {
    "attrs_block",
    "colon_fence",
    "html_admonition",
    "replacements",
    "smartquotes",
    "strikethrough",
    "tasklist",
}
rust_crates = {"rust_msg": "../msg", "rust_client": "../client"}
rust_doc_dir = "sphinx/crates/"
rust_rustdoc_fmt = "md"
breathe_default_project = "cpp_seanerbus"

templates_path = ["_templates"]
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]
