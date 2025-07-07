#ifndef UISTYLEHELPER_H
#define UISTYLEHELPER_H

#include <Wt/WWidget.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTable.h>
#include <Wt/WText.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WGroupBox.h>
#include <string>

/**
 * @class UIStyleHelper
 * @brief Centralized styling utility for consistent CSS class application
 * 
 * This helper ensures all components use consistent Bootstrap classes
 * and are properly themed across the entire POS system.
 */
class UIStyleHelper {
public:
    // =================================================================
    // STANDARD COMPONENT STYLES
    // =================================================================
    
    /**
     * @brief Apply standard container styling
     */
    static void styleContainer(Wt::WContainerWidget* container, const std::string& variant = "default") {
        if (!container) return;
        
        // Base container classes
        container->addStyleClass("pos-container");
        
        if (variant == "panel") {
            container->addStyleClass("panel-container p-3 mb-4 bg-white border rounded-3 shadow-sm");
        } else if (variant == "header") {
            container->addStyleClass("header-container p-3 mb-3 bg-primary text-white rounded-top");
        } else if (variant == "content") {
            container->addStyleClass("content-container p-3");
        } else if (variant == "sidebar") {
            container->addStyleClass("sidebar-container p-2 bg-light border-end");
        } else {
            container->addStyleClass("d-flex flex-column gap-2");
        }
    }
    
    /**
     * @brief Apply standard group box styling
     */
    static void styleGroupBox(Wt::WGroupBox* groupBox, const std::string& variant = "default") {
        if (!groupBox) return;
        
        groupBox->addStyleClass("pos-group-box");
        
        if (variant == "primary") {
            groupBox->addStyleClass("border-primary bg-light");
        } else if (variant == "success") {
            groupBox->addStyleClass("border-success bg-success-subtle");
        } else if (variant == "warning") {
            groupBox->addStyleClass("border-warning bg-warning-subtle");
        } else {
            groupBox->addStyleClass("border rounded-3 bg-white shadow-sm");
        }
        
        // Add consistent padding
        groupBox->addStyleClass("p-3 mb-3");
    }
    
    /**
     * @brief Apply standard table styling
     */
    static void styleTable(Wt::WTable* table, const std::string& variant = "default") {
        if (!table) return;
        
        // Base table classes
        table->addStyleClass("table pos-table");
        
        if (variant == "orders") {
            table->addStyleClass("table-striped table-hover table-bordered");
            table->addStyleClass("table-responsive-md");
        } else if (variant == "menu") {
            table->addStyleClass("table-striped table-hover");
            table->addStyleClass("table-responsive-lg");
        } else if (variant == "compact") {
            table->addStyleClass("table-sm table-striped");
        } else {
            table->addStyleClass("table-striped table-hover");
        }
        
        // Consistent width
        table->setWidth(Wt::WLength("100%"));
    }
    
    /**
     * @brief Apply standard button styling
     */
    static void styleButton(Wt::WPushButton* button, const std::string& variant = "primary", const std::string& size = "md") {
        if (!button) return;
        
        // Remove any existing button classes
        button->removeStyleClass("btn");
        
        // Base button class
        button->addStyleClass("btn pos-button");
        
        // Variant classes
        if (variant == "primary") {
            button->addStyleClass("btn-primary");
        } else if (variant == "secondary") {
            button->addStyleClass("btn-secondary");
        } else if (variant == "success") {
            button->addStyleClass("btn-success");
        } else if (variant == "warning") {
            button->addStyleClass("btn-warning");
        } else if (variant == "danger") {
            button->addStyleClass("btn-danger");
        } else if (variant == "outline-primary") {
            button->addStyleClass("btn-outline-primary");
        } else if (variant == "outline-secondary") {
            button->addStyleClass("btn-outline-secondary");
        } else if (variant == "outline-danger") {
            button->addStyleClass("btn-outline-danger");
        } else {
            button->addStyleClass("btn-primary");
        }
        
        // Size classes
        if (size == "sm") {
            button->addStyleClass("btn-sm");
        } else if (size == "lg") {
            button->addStyleClass("btn-lg");
        }
        
        // Add consistent spacing
        button->addStyleClass("me-2 mb-2");
    }
    
    /**
     * @brief Apply standard form control styling
     */
    static void styleFormControl(Wt::WWidget* widget, const std::string& size = "md") {
        if (!widget) return;
        
        widget->addStyleClass("form-control pos-form-control");
        
        if (size == "sm") {
            widget->addStyleClass("form-control-sm");
        } else if (size == "lg") {
            widget->addStyleClass("form-control-lg");
        }
        
        widget->addStyleClass("mb-2");
    }
    
    /**
     * @brief Apply standard combo box styling
     */
    static void styleComboBox(Wt::WComboBox* combo, const std::string& size = "md") {
        if (!combo) return;
        
        combo->addStyleClass("form-select pos-select");
        
        if (size == "sm") {
            combo->addStyleClass("form-select-sm");
        } else if (size == "lg") {
            combo->addStyleClass("form-select-lg");
        }
        
        combo->addStyleClass("mb-2");
    }
    
    // =================================================================
    // TEXT STYLING
    // =================================================================
    
    /**
     * @brief Apply standard heading styling
     */
    static void styleHeading(Wt::WText* text, int level = 4, const std::string& color = "default") {
        if (!text) return;
        
        // Heading level
        text->addStyleClass("h" + std::to_string(level));
        text->addStyleClass("pos-heading");
        
        // Color variants
        if (color == "primary") {
            text->addStyleClass("text-primary");
        } else if (color == "success") {
            text->addStyleClass("text-success");
        } else if (color == "warning") {
            text->addStyleClass("text-warning");
        } else if (color == "danger") {
            text->addStyleClass("text-danger");
        } else if (color == "muted") {
            text->addStyleClass("text-muted");
        } else if (color == "white") {
            text->addStyleClass("text-white");
        }
        
        // Consistent margins
        text->addStyleClass("mb-3");
    }
    
    /**
     * @brief Apply standard badge styling
     */
    static void styleBadge(Wt::WText* text, const std::string& variant = "secondary") {
        if (!text) return;
        
        text->addStyleClass("badge pos-badge");
        
        if (variant == "primary") {
            text->addStyleClass("bg-primary");
        } else if (variant == "secondary") {
            text->addStyleClass("bg-secondary");
        } else if (variant == "success") {
            text->addStyleClass("bg-success");
        } else if (variant == "warning") {
            text->addStyleClass("bg-warning text-dark");
        } else if (variant == "danger") {
            text->addStyleClass("bg-danger");
        } else if (variant == "info") {
            text->addStyleClass("bg-info text-dark");
        }
        
        text->addStyleClass("fs-6");
    }
    
    /**
     * @brief Apply status indicator styling
     */
    static void styleStatusText(Wt::WText* text, const std::string& status) {
        if (!text) return;
        
        text->addStyleClass("pos-status small fw-bold");
        
        if (status == "success") {
            text->addStyleClass("text-success");
        } else if (status == "warning") {
            text->addStyleClass("text-warning");
        } else if (status == "error" || status == "danger") {
            text->addStyleClass("text-danger");
        } else if (status == "info") {
            text->addStyleClass("text-info");
        } else {
            text->addStyleClass("text-muted");
        }
    }
    
    // =================================================================
    // LAYOUT HELPERS
    // =================================================================
    
    /**
     * @brief Create flex row container
     */
    static void styleFlexRow(Wt::WContainerWidget* container, const std::string& justify = "start", const std::string& align = "center") {
        if (!container) return;
        
        container->addStyleClass("d-flex flex-row");
        
        // Justify content
        if (justify == "center") {
            container->addStyleClass("justify-content-center");
        } else if (justify == "end") {
            container->addStyleClass("justify-content-end");
        } else if (justify == "between") {
            container->addStyleClass("justify-content-between");
        } else if (justify == "around") {
            container->addStyleClass("justify-content-around");
        }
        
        // Align items
        if (align == "start") {
            container->addStyleClass("align-items-start");
        } else if (align == "end") {
            container->addStyleClass("align-items-end");
        } else if (align == "center") {
            container->addStyleClass("align-items-center");
        } else if (align == "stretch") {
            container->addStyleClass("align-items-stretch");
        }
        
        container->addStyleClass("gap-2");
    }
    
    /**
     * @brief Create flex column container
     */
    static void styleFlexColumn(Wt::WContainerWidget* container, const std::string& justify = "start") {
        if (!container) return;
        
        container->addStyleClass("d-flex flex-column gap-3");
        
        if (justify == "center") {
            container->addStyleClass("justify-content-center");
        } else if (justify == "end") {
            container->addStyleClass("justify-content-end");
        } else if (justify == "between") {
            container->addStyleClass("justify-content-between");
        }
    }
    
    // =================================================================
    // COMPONENT-SPECIFIC STYLES
    // =================================================================
    
    /**
     * @brief Style order display components
     */
    static void styleOrderDisplay(Wt::WContainerWidget* container) {
        styleContainer(container, "panel");
        container->addStyleClass("order-display-container");
    }
    
    /**
     * @brief Style menu display components
     */
    static void styleMenuDisplay(Wt::WContainerWidget* container) {
        styleContainer(container, "panel");
        container->addStyleClass("menu-display-container");
    }
    
    /**
     * @brief Style kitchen display components
     */
    static void styleKitchenDisplay(Wt::WContainerWidget* container) {
        styleContainer(container, "panel");
        container->addStyleClass("kitchen-display-container");
    }
    
    /**
     * @brief Style header components
     */
    static void styleHeaderComponent(Wt::WContainerWidget* container) {
        styleContainer(container, "header");
        container->addStyleClass("pos-header");
    }
    
    /**
     * @brief Style footer components
     */
    static void styleFooterComponent(Wt::WContainerWidget* container) {
        container->addStyleClass("pos-footer p-2 bg-light border-top text-muted small");
        styleFlexRow(container, "between", "center");
    }
    
    // =================================================================
    // RESPONSIVE UTILITIES
    // =================================================================
    
    /**
     * @brief Add responsive visibility classes
     */
    static void addResponsiveVisibility(Wt::WWidget* widget, bool hideOnMobile = false, bool hideOnTablet = false) {
        if (!widget) return;
        
        if (hideOnMobile) {
            widget->addStyleClass("d-none d-md-block");
        }
        if (hideOnTablet) {
            widget->addStyleClass("d-none d-lg-block");
        }
    }
    
    /**
     * @brief Add responsive spacing
     */
    static void addResponsiveSpacing(Wt::WWidget* widget) {
        if (!widget) return;
        widget->addStyleClass("p-2 p-md-3 p-lg-4");
    }
    
    // =================================================================
    // THEME-AWARE STYLING
    // =================================================================
    
    /**
     * @brief Apply theme-aware container styling
     */
    static void applyThemeAwareStyle(Wt::WWidget* widget, const std::string& themeClass = "") {
        if (!widget) return;
        
        widget->addStyleClass("pos-theme-aware");
        
        if (!themeClass.empty()) {
            widget->addStyleClass(themeClass);
        }
    }
    
    // =================================================================
    // ANIMATION CLASSES
    // =================================================================
    
    /**
     * @brief Add fade-in animation
     */
    static void addFadeIn(Wt::WWidget* widget, int delay = 0) {
        if (!widget) return;
        widget->addStyleClass("pos-fade-in");
        if (delay > 0) {
            widget->addStyleClass("pos-delay-" + std::to_string(delay));
        }
    }
    
    /**
     * @brief Add hover effects
     */
    static void addHoverEffect(Wt::WWidget* widget, const std::string& effect = "shadow") {
        if (!widget) return;
        
        if (effect == "shadow") {
            widget->addStyleClass("pos-hover-shadow");
        } else if (effect == "lift") {
            widget->addStyleClass("pos-hover-lift");
        } else if (effect == "glow") {
            widget->addStyleClass("pos-hover-glow");
        }
    }
};

#endif // UISTYLEHELPER_H
