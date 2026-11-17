// SPDX-License-Identifier: GPL-2.0-only
/*
 * banana-pocket: board identity for the Banana Pocket handheld.
 *
 * One platform device per board, bound to the "banana,pocket" DT node.
 * Read-only attributes under /sys/devices/platform/banana-pocket/:
 *   model            DT root "model" string
 *   board            "banana,board" property of the node (e.g. "classic")
 *   board_id         first cell of the root "qcom,board-id" property, hex
 *   kernel_version   running kernel release (includes the Banana LOCALVERSION)
 */
#include <linux/device.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/sysfs.h>
#include <linux/utsname.h>

struct banana_pocket {
	const char *model;
	const char *board;
	u32 board_id;
};

static ssize_t model_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct banana_pocket *bp = dev_get_drvdata(dev);

	return sysfs_emit(buf, "%s\n", bp->model);
}
static DEVICE_ATTR_RO(model);

static ssize_t board_show(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct banana_pocket *bp = dev_get_drvdata(dev);

	return sysfs_emit(buf, "%s\n", bp->board);
}
static DEVICE_ATTR_RO(board);

static ssize_t board_id_show(struct device *dev, struct device_attribute *attr,
			     char *buf)
{
	struct banana_pocket *bp = dev_get_drvdata(dev);

	return sysfs_emit(buf, "0x%x\n", bp->board_id);
}
static DEVICE_ATTR_RO(board_id);

static ssize_t kernel_version_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "%s\n", init_utsname()->release);
}
static DEVICE_ATTR_RO(kernel_version);

static struct attribute *banana_pocket_attrs[] = {
	&dev_attr_model.attr,
	&dev_attr_board.attr,
	&dev_attr_board_id.attr,
	&dev_attr_kernel_version.attr,
	NULL
};
ATTRIBUTE_GROUPS(banana_pocket);

static int banana_pocket_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct banana_pocket *bp;
	int ret;

	bp = devm_kzalloc(dev, sizeof(*bp), GFP_KERNEL);
	if (!bp)
		return -ENOMEM;

	ret = of_property_read_string(of_root, "model", &bp->model);
	if (ret)
		return dev_err_probe(dev, ret, "no root model\n");

	ret = of_property_read_string(dev->of_node, "banana,board", &bp->board);
	if (ret)
		return dev_err_probe(dev, ret, "no banana,board\n");

	ret = of_property_read_u32(of_root, "qcom,board-id", &bp->board_id);
	if (ret)
		return dev_err_probe(dev, ret, "no qcom,board-id\n");

	dev_set_drvdata(dev, bp);
	dev_info(dev, "%s (%s), board id 0x%x\n", bp->model, bp->board,
		 bp->board_id);
	return 0;
}

static const struct of_device_id banana_pocket_of_match[] = {
	{ .compatible = "banana,pocket" },
	{ }
};
MODULE_DEVICE_TABLE(of, banana_pocket_of_match);

static struct platform_driver banana_pocket_driver = {
	.probe = banana_pocket_probe,
	.driver = {
		.name = "banana-pocket",
		.of_match_table = banana_pocket_of_match,
		.dev_groups = banana_pocket_groups,
	},
};
module_platform_driver(banana_pocket_driver);

MODULE_DESCRIPTION("Banana Pocket board identity");
MODULE_AUTHOR("Banana");
MODULE_LICENSE("GPL");
