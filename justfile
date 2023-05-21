# List all recipes
list:
	@just --list

# Set up SPEC CPU2006
setup-spec2006:
	./scripts/setup-spec2006.sh

# Regenerate patches for SPEC CPU2006
regen-spec2006:
	./scripts/regen-spec2006.sh
