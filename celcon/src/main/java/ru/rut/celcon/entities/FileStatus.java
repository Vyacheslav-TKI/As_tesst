package ru.rut.celcon.entities;

public enum FileStatus {
    PENDING("ПРОВЕРЯЕТСЯ", "pending"),
    CHANGED("ИЗМЕНЁН", "changed"),
    UNCHANGED("НЕ ИЗМЕНЁН", "unchanged");

    private final String displayName;
    private final String cssClass;

    FileStatus(String displayName, String cssClass) {
        this.displayName = displayName;
        this.cssClass = cssClass;
    }

    public String getDisplayName() {
        return displayName;
    }

    public String getCssClass() {
        return cssClass;
    }
}
