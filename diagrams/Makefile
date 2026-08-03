#
# Generate PlantUML diagrams as SVG, then convert to PDF with Inkscape.
#

PUML_FILES := $(wildcard src/*.puml)
DIAGRAMS   := $(patsubst src/%.puml,%,$(PUML_FILES))

SVG_DIR := build/svg
PDF_DIR := build/pdf

SVG_FILES := $(addprefix $(SVG_DIR)/,$(addsuffix .svg,$(DIAGRAMS)))
PDF_FILES := $(addprefix $(PDF_DIR)/,$(addsuffix .pdf,$(DIAGRAMS)))

.PHONY: all svg pdf clean

all: svg pdf

svg: $(SVG_FILES)

pdf: $(PDF_FILES)

$(SVG_DIR) $(PDF_DIR):
	@mkdir -p $@

$(SVG_DIR)/%.svg: src/%.puml | $(SVG_DIR)
	@echo "PlantUML $< -> $@"
	@plantuml -tsvg -pipe < $< > $@

$(PDF_DIR)/%.pdf: $(SVG_DIR)/%.svg | $(PDF_DIR)
	@echo "Inkscape $< -> $@"
	@inkscape $< --export-type=pdf --export-filename=$@

clean:
	@rm -rf $(SVG_DIR) $(PDF_DIR)
